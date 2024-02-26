#include "label.h"
#include "glyph_texture.h"
#include "camera.h"
#include "utils.h"
#include "shader.h"
#include "macros.h"

#include <cassert>

#include <glad/glad.h>

#include <filesystem>
#include <freetype/freetype.h>
#include <freetype/ftstroke.h>

#include "color_rect.h"
#include "editor.h"

extern Editor* myEditor;

Label::Label(Camera* _cam, std::wstring _text) : m_camera(_cam), m_text(_text) {
	m_shader = new Shader(readFile(GLYPH_VERTEX_SHADER_PATH).c_str(), readFile(GLYPH_FRAGMENT_SHADER_PATH).c_str());
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	ColorRect* baseCR = new ColorRect(m_camera);
	baseCR->setSize(vec2i(0, FONT_SIZE));
	baseCR->setPosition(vec2(m_position.x + baseCR->getSize().x / 2.0f, m_position.y + 10));
	baseCR->setColor(m_selectionColor);
	m_selectionList.push_back(baseCR);

	FT_Library ft;
	FT_Init_FreeType(&ft);
	FT_Face face;
	if (FT_New_Face(ft, FONT_PATH, 0, &face)) {
		PUSH_ERROR("Cannot Make New Freetype Face");
		FT_Done_FreeType(ft);
		return;
	}
	FT_Set_Pixel_Sizes(face, 0, FONT_SIZE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (wchar_t c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			PUSH_ERROR("Cannot Make New Glyph");
			continue;
		}
		if (c == L'\n') {
			m_glyphTextureMap[c] = nullptr;
			continue;
		}
		GlyphTexture* glyphTex = 
			new GlyphTexture(
				c,
				face->glyph->bitmap.buffer,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				1,
				vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<unsigned int>(face->glyph->advance.x),
				true
			);
		m_glyphTextureMap[c] = glyphTex;
	}

	if (m_text == L"") return;
	m_size = vec2();
	float lastMaxWidth = 0.0f;
	float lastMaxHeight = 0.0f;

	for (size_t i = 0; i < m_text.size(); i++) {
		if (m_text[i] == L'\n') {
			if (lastMaxWidth > m_size.x) {
				m_size.x = lastMaxWidth;
			}
			m_size.y += lastMaxHeight;
			lastMaxWidth = 0.0f;
			lastMaxHeight = 0.0f;
			m_escapeSequenceCount++;
			m_glyphTextures.push_back(std::nullopt);
		}
		else {
			m_glyphTextures.push_back(m_glyphTextureMap[m_text[i]]);
			m_size.x += (m_glyphTextureMap[m_text[i]]->getAdvanceX() >> 6);
			lastMaxWidth += (m_glyphTextureMap[m_text[i]]->getAdvanceX() >> 6);
			if (lastMaxHeight < m_glyphTextureMap[m_text[i]]->getSize().y) {
				lastMaxHeight = m_glyphTextureMap[m_text[i]]->getSize().y;
			}
			if (i + 1 <= m_glyphTextures.size()) {
				m_camera->addZoom(vec2(-0.01f));
			}
		}
	}
	if (m_size.y == 0.0f) {
		m_size.y = lastMaxHeight;
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	updateHighlight();
	for (size_t i = 0; i < m_escapeSequenceCount; i++) {
		ColorRect* c = nullptr;
		c = new ColorRect(m_camera);
		c->setSize(vec2i(0, FONT_SIZE));
		c->setPosition(vec2(m_position.x + c->getSize().x / 2.0f, m_position.y + (FONT_SIZE * i + 1) + 10));
		c->setColor(m_selectionColor);
		m_selectionList.push_back(c);
	}
	updateSelection(0, 0);
}

Label::~Label() {
	for (auto& tex : m_glyphTextureMap) {
		if (tex.second != nullptr) {
			delete tex.second;
		}
	}
	for (auto& sel : m_selectionList) {
		delete sel;
	}
}

void Label::update() {

}

void Label::draw() const {
	glSetRenderMode(GLRenderMode::GL2D);
	int screenWidth = myEditor->windowSize.x;
	int screenHeight = myEditor->windowSize.y;
	float x = m_position.x;
	float y = -m_position.y - FONT_SIZE / 2;
	glBindVertexArray(m_VAO);
	m_shader->use();
	m_shader->setMat4("ViewMatrix", m_camera->getViewMatrix().data());
	m_shader->setMat4("ProjectionMatrix", m_camera->getProjectionMatrix().data());
	m_shader->setVec4("textColor", m_color);
	m_shader->setFloat("Time", glfwGetTime());
	m_shader->setBool("Rainbow_Enabled", m_enableRainbow);
	std::vector<SyntaxHighlight> tempHighlightList = m_higilightList;
	SyntaxHighlight* currentHighlight = nullptr;
	for (size_t index = 0; index < m_text.size(); index++) {
		if (!m_enableRainbow) {
			if (!tempHighlightList.empty() && tempHighlightList[0].Start == index) {
				currentHighlight = &tempHighlightList[0];
			}
			if (currentHighlight != nullptr) {
				m_shader->setVec4("textColor", currentHighlight->Color);
				if (currentHighlight->End - 1 == index) {
					currentHighlight = nullptr;
					tempHighlightList.erase(tempHighlightList.begin());
				}
			}
			else {
				m_shader->setVec4("textColor", m_color);
			}
		}
		GlyphTexture* ch = nullptr;
		if (m_glyphTextures[index].has_value()) {
			if (m_glyphTextures[index].value() != nullptr) {
				ch = m_glyphTextures[index].value();
			}
		}
		else {
			x = m_position.x;
			y -= FONT_SIZE;
			continue;
		}
		if (ch != nullptr) {
			ch->bind();
			float xpos = (x + ch->getBearing().x);
			float ypos = y - (ch->getSize().y - ch->getBearing().y);
			float w = ch->getSize().x;
			float h = ch->getSize().y;

			float vertices[6][4] = {
				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos,     ypos,       0.0f, 1.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },

				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },
				{ xpos + w, ypos + h,   1.0f, 0.0f }
			};
			glBindTexture(GL_TEXTURE_2D, ch->getID());
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			x += (ch->getAdvanceX() >> 6);
		}
	}
	m_shader->unuse();
	glBindVertexArray(0);
	for (auto& sel : m_selectionList) {
		sel->draw();
	}
}

void Label::push_back(const wchar_t& ch) {
	insert(m_text.size() - 1, ch);
}

void Label::insert(const size_t& at, const wchar_t& ch) {
	m_text.insert(m_text.begin() + at, ch);
	if (ch == L'\n') {
		m_escapeSequenceCount++;
		m_glyphTextures.insert(m_glyphTextures.begin() + at, std::nullopt);
		m_size.y += FONT_SIZE;
		updateBlockList();
		updateHighlight();
		return;
	}
	if (m_glyphTextureMap.find(ch) == m_glyphTextureMap.end()) {
		return;
	}
	GlyphTexture* tex = m_glyphTextureMap[ch];
	m_size.x += (tex->getAdvanceX() >> 6);
	m_glyphTextures.insert(m_glyphTextures.begin() + at, tex);
	updateHighlight();
	updateBlockList();
}


void Label::erase(const size_t& at) {
	if (!(at >= 0 && at < m_text.size())) {
		return;
	}
	m_text.erase(m_text.begin() + at);
	if (m_glyphTextures[m_glyphTextures.size() - 1].has_value()){
		m_size.x -= (m_glyphTextures[m_glyphTextures.size() - 1].value()->getAdvanceX() >> 6);
	}
	else {
		m_size.y -= FONT_SIZE;
		m_escapeSequenceCount--;
	}
	m_glyphTextures.erase(m_glyphTextures.begin() + at);
	updateHighlight();
	updateBlockList();
}

void Label::pop_back() {
	if (m_text.empty()) return;
	m_text.pop_back();
	m_glyphTextures.pop_back();
	updateHighlight();
	updateBlockList();
}

const std::vector<std::optional<GlyphTexture*>>& Label::getGlyphTextures() {
	return m_glyphTextures;
}

void Label::setPosition(const vec2& _pos) {
	m_position = _pos;
}
const vec2& Label::getPosition() {
	return m_position;
}
const vec2& Label::getSize() {
	return m_size;
}

const size_t& Label::getEscapeSequenceCount() {
	return m_escapeSequenceCount;
}


void Label::updateBlockList() {
	m_blockList.clear();
	bool first_initialized = false;
	std::pair<int, int> holder{0, 0};
	for (size_t i = 0; i < m_glyphTextures.size(); i++) {
		if (m_glyphTextures[i].has_value()) {
			if (!first_initialized) {
				first_initialized = true;
				holder.first = i;
			}
			else {
				holder.second = i;
			}
			continue;
		}
		if (!first_initialized) 
		{
			holder = { holder.first, holder.first };
			m_blockList.emplace_back(holder);
			holder = { holder.first + 1, 0 };
			first_initialized = false;
			continue;
		}
		holder.second += 1;
		if (holder.second == 1) {
			holder.second += holder.first;
		}
		m_blockList.emplace_back(holder);
		holder = { holder.second + 1, 0 };
		first_initialized = false;
	}
	if (first_initialized) {
		holder.second = m_glyphTextures.size();
		m_blockList.emplace_back(holder);
	}
	else if (m_blockList.size() != m_glyphTextures.size()) {
		holder.second = m_glyphTextures.size();
		m_blockList.push_back(holder);
	}
}

int Label::getBelongBlock(const int& at) {
	updateBlockList();
	if (m_blockList.size() == 0) {
		return 0;
	}
	for (int i = 0; i < m_blockList.size(); i++) {
		if (m_blockList[i].first <= at && at <= m_blockList[i].second) {
			return i;
		}
	}
	return -1;
}

int Label::getLongestBlock() {
	size_t result = 0;
	int value = -1;
	for (size_t i = 0; i < m_blockList.size(); i++) {
		if (m_blockList[i].second - m_blockList[i].first > value) {
			value = m_blockList[i].second - m_blockList[i].first;
			result = i;
		}
	}
	return result;
}

const std::vector<std::pair<int, int>>& Label::getBlockList() {
	return m_blockList;
}


void Label::updateHighlight() {
	m_higilightList = {};
	size_t index = 0;
	while (index < m_text.size()) {
		if (isAlphabet(m_text[index])) {
			size_t start = index;
			std::wstring keyword = L"";
			while ((isAlphabet(m_text[index]) || isNumber(m_text[index])) && index < m_text.size()) {
				keyword += m_text[index];
				index++;
			}
			size_t end = index;
			if (m_Keywords.find(keyword) != m_Keywords.end()) {
				SyntaxHighlight s;
				s.Color = m_Keywords[keyword];
				s.Start = start;
				s.End = end;
				m_higilightList.push_back(s);
			}
#if TARGET_LANG == TARGET_LANG_TYPE_CPP || TARGET_LANG == TARGET_LANG_TYPE_JAVASCRIPT
			else {
				size_t functionStart = start;
				while ((isAlphabet(m_text[index]) || isNumber(m_text[index])) && index < m_text.size()) {
					index++;
				}
				size_t functionEnd = index;
				if (m_text[index] == L'(') {
					SyntaxHighlight s;
					s.Color = m_FunctionColor;
					s.Start = functionStart;
					s.End = functionEnd;
					m_higilightList.push_back(s);
				}
			}
#endif
			continue;
		}
		else if (isNumber(m_text[index])) {
			size_t start = index;
			std::wstring keyword = L"";
			while (isNumber(m_text[index]) && index < m_text.size()) {
				keyword += m_text[index];
				index++;
			}
			if (m_text[index] == L'.') {
				index++;
				while (isNumber(m_text[index]) && index < m_text.size()) {
					keyword += m_text[index];
					index++;
				}
#if TARGET_LANG == TARGET_LANG_TYPE_CPP
				if (m_text[index] == L'f' || m_text[index] == L'F') {
					index++;
				}
				else if (m_text[index] == L'u' || m_text[index] == L'U') {
					index++;
				}
				else if (m_text[index] == L'l' || m_text[index] == L'L') {
					index++;
				}
#endif
			}
			size_t end = index;
			SyntaxHighlight s;
			s.Color = m_NumberLiteralColor;
			s.Start = start;
			s.End = end;
			m_higilightList.push_back(s);
			continue;
		}
#if TARGET_LANG == TARGET_LANG_TYPE_CPP || TARGET_LANG == TARGET_LANG_TYPE_JAVASCRIPT
		else if (m_text[index] == L'/') {
			if (index + 1 < m_text.size()) {
				if (m_text[index + 1] == L'/') {
					size_t start = index;
					index++;
					while (m_text[index] != L'\n' && index < m_text.size()) {
						index++;
					}
					size_t end = index;
					SyntaxHighlight s;
					s.Color = m_CommentColor;
					s.Start = start;
					s.End = end;
					m_higilightList.push_back(s);
					continue;
				}
				else if (m_text[index + 1] == L'*') {
					size_t start = index;
					index++;
					while (!(m_text[index] == L'*' && index + 1 < m_text.size() && m_text[index + 1] == L'/') && index < m_text.size()) {
						index++;
					}
					size_t end = index;
					end += 2;
					SyntaxHighlight s;
					s.Color = m_CommentColor;
					s.Start = start;
					s.End = end;
					m_higilightList.push_back(s);
					continue;
				}
			}
			index++;
			continue;
		}
#endif
		else if (m_text[index] == L'"' && index + 1 < m_text.size()) {
			size_t start = index;
			index++;
			while (m_text[index] != L'"' && index < m_text.size()) {
				index++;
			}
			index++;
			size_t end = index;
			SyntaxHighlight s;
			s.Color = m_StringLiteralColor;
			s.Start = start;
			s.End = end;
			m_higilightList.push_back(s);
			continue;
		}
		else if (m_text[index] == L'\'' && index + 1 < m_text.size()) {
			size_t start = index;
			index++;
			while (m_text[index] != L'\'' && index < m_text.size()) {
				index++;
			}
			index++;
			size_t end = index;
			SyntaxHighlight s;
			s.Color = m_StringLiteralColor;
			s.Start = start;
			s.End = end;
			m_higilightList.push_back(s);
			continue;
		}
		else if (m_text[index] == L'#') {
#if TARGET_LANG == TARGET_LANG_TYPE_CPP
			size_t start = index;
			std::wstring preprocessor = L"";
			while (!(m_text[index] == L'\n' || m_text[index] == L' ') && index < m_text.size()) {
				index++;
				preprocessor += m_text[index];
			}
			bool parseHeader = m_text[index] == L' ';
			index++;
			size_t end = index;
			SyntaxHighlight s;
			s.Color = m_PreprocessorColor;
			s.Start = start;
			s.End = end;
			m_higilightList.push_back(s);
			if (parseHeader && m_text[index] == L'<') {
				size_t headerStart = index;
				while (m_text[index] != L'\n' && index < m_text.size()) {
					index++;
				}
				size_t headerEnd = index;
				if ( headerStart < headerEnd) {
					SyntaxHighlight ss;
					ss.Color = m_StringLiteralColor;
					ss.Start = headerStart;
					ss.End = headerEnd;
					m_higilightList.push_back(ss);
				}
			}
#elif TARGET_LANG == TARGET_LANG_TYPE_PYTHON
			size_t start = index;
			std::wstring comment = L"";
			while (m_text[index] != L'\n' && index < m_text.size()) {
				index++;
				comment += m_text[index];
			}
			bool parseHeader = m_text[index] == L' ';
			index++;
			size_t end = index;
			SyntaxHighlight s;
			s.Color = m_CommentColor;
			s.Start = start;
			s.End = end;
			m_higilightList.push_back(s);
#endif
		}
		else {
			index++;
		}
	}
}

const std::wstring& Label::getText() {
	return m_text;
}

void Label::updateSelection(const size_t& from, const size_t& to) {
	assert(0 <= from && from <= m_text.size());
	assert(0 <= to && to <= m_text.size());
	int activatedBlockStart = getBelongBlock(from);
	int activatedBlockEnd = getBelongBlock(to);
	if (activatedBlockStart == -1 || activatedBlockEnd == -1) {
		return;
	}
	if (m_selectionList.size() < activatedBlockEnd) {
		return;
	}
	for (auto& cr : m_selectionList) {
		cr->setSize(vec2i(0, cr->getSize().y));
	}
	if (m_blockList.empty()) {
		return;
	}
	{
		int firstStart = from;
		int firstEnd = from;

		while (firstEnd < m_blockList[activatedBlockStart].second) {
			firstEnd++;
		}
		if (firstEnd > to) {
			firstEnd = to;
		}
		int posX = 0;
		int posY = 10 + FONT_SIZE * (activatedBlockStart);
		int sizeX = 0;
		const int sizeY = FONT_SIZE;
		for (int index = 0; index < firstStart; index++) {
			if (m_glyphTextures[index].has_value() && m_glyphTextures[index].value() != nullptr) {
				posX += (m_glyphTextures[index].value()->getAdvanceX() >> 6);
			}
			else {
				posX = 0;
			}
		}
		for (int index = firstStart; index < firstEnd; index++) {
			if (m_glyphTextures[index].has_value() && m_glyphTextures[index].value() != nullptr) {
				sizeX += (m_glyphTextures[index].value()->getAdvanceX() >> 6);
			}
		}

		m_selectionList[0]->setSize(vec2i(sizeX, sizeY));
		m_selectionList[0]->setPosition(vec2(m_position.x + m_selectionList[0]->getSize().x / 2.0f + posX, posY));
	}
	{
		for (int i = activatedBlockStart + 1; i < activatedBlockEnd; i++) {
			int middleStart = m_blockList[i].first;
			int middleEnd = m_blockList[i].second;
			int posX = 0;
			int posY = 10 + FONT_SIZE * i;
			int sizeX = 0;
			const int sizeY = FONT_SIZE;

			for (int index = middleStart; index < middleEnd; index++) {
				if (m_glyphTextures[index].has_value() && m_glyphTextures[index].value() != nullptr) {
					sizeX += (m_glyphTextures[index].value()->getAdvanceX() >> 6);
				}
			}

			m_selectionList[i]->setSize(vec2i(sizeX, sizeY));
			m_selectionList[i]->setPosition(vec2(m_position.x + m_selectionList[i]->getSize().x / 2.0f + posX, posY));
		}
	}
	if (activatedBlockStart != activatedBlockEnd && m_blockList.size() > activatedBlockEnd){
		int lastStart = to;
		int lastEnd = to;
		while (lastStart > m_blockList[activatedBlockEnd].first) {
			lastStart--;
		}

		int posX = 0;
		int posY = 10 + FONT_SIZE * (activatedBlockEnd);
		int sizeX = 0;
		const int sizeY = FONT_SIZE;

		for (int index = lastStart; index < lastEnd; index++) {
			if (m_glyphTextures[index].has_value() && m_glyphTextures[index].value() != nullptr) {
				sizeX += (m_glyphTextures[index].value()->getAdvanceX() >> 6);
			}
		}

		m_selectionList[m_selectionList.size()-1]->setSize(vec2i(sizeX, sizeY));
		m_selectionList[m_selectionList.size()-1]->setPosition(vec2(m_position.x + m_selectionList[m_selectionList.size()-1]->getSize().x / 2.0f + posX, posY));
	}
}

void Label::addSelectionSection() {
	ColorRect* c = nullptr;
	c = new ColorRect(m_camera);
	c->setSize(vec2i(0, FONT_SIZE));
	c->setPosition(vec2(m_position.x + c->getSize().x / 2.0f, m_position.y + (FONT_SIZE * m_escapeSequenceCount) + 10));
	c->setColor(m_selectionColor);
	m_selectionList.push_back(c);
}
void Label::eraseSelectionSection(const int& at) {
	if (at < 0 || at >= m_selectionList.size()) {
		return;
	}
	if (m_selectionList.size() == 1) {
		return;
	}
	if (m_selectionList.size() == at + 1) {
		delete m_selectionList[at];
		m_selectionList.erase(m_selectionList.begin() + at);
		return;
	}
	if (m_selectionList.size() == at) {
		delete m_selectionList[at - 1];
		m_selectionList.erase(m_selectionList.begin() + at - 1);
		return;
	}
	delete m_selectionList[at + 1];
	m_selectionList.erase(m_selectionList.begin() + at + 1);
}

void Label::toggleRainbow() {
	m_enableRainbow = !m_enableRainbow;
}