#ifndef LABEL_H
#define LABEL_H

#include <vector>
#include <string>
#include <optional>
#include "math_utils.h"
#include "utils.h"
#include "macros.h"

#include <stb_image.h>

struct SyntaxHighlight {
	int Start;
	int End;
	vec4 Color;
};


class Label final {
private:
#if TARGET_LANG == TARGET_LANG_TYPE_CPP
	vec4 m_NumberLiteralColor = hex2rgba(0xb5cea8);
	vec4 m_CommentColor = hex2rgba(0x57a64a);
	vec4 m_StringLiteralColor = hex2rgba(0xd69d85);
	vec4 m_PreprocessorColor = hex2rgba(0x9b9b9b);
	vec4 m_KeywordDefaultColor = hex2rgba(0x569cd6);
	vec4 m_KeywordSpecialColor = hex2rgba(0xd8a0df);
	vec4 m_FunctionColor = hex2rgba(0xd4b964);
	vec4 m_MacroColor = hex2rgba(0xbeb7ff);
	std::map<std::wstring, vec4> m_Keywords = {
		{L"NULL", m_MacroColor},
		{L"TRUE", m_MacroColor},
		{L"FALSE", m_MacroColor},
		{L"EXIT_SUCCESS", m_MacroColor},
		{L"EXIT_FAILIURE", m_MacroColor},
		{L"__FILE__", m_MacroColor},
		{L"__LINE__", m_MacroColor},
		{L"__FUNCTION__", m_MacroColor},
		{L"__DATE__", m_MacroColor},
		{L"__TIME__", m_MacroColor},
		{L"__STDC__", m_MacroColor},
		{L"__STDC_VERSION__", m_MacroColor},
		{L"__STDC_HOSTED__", m_MacroColor},
		{L"__cplusplus", m_MacroColor},
		{L"__OBJC__", m_MacroColor},
		{L"__ASSEMBLER__", m_MacroColor},
		{L"__cdecl", m_KeywordDefaultColor},
		{L"__thiscall", m_KeywordDefaultColor},
		{L"__stdcall", m_KeywordDefaultColor},
		{L"__fastcall", m_KeywordDefaultColor},
		{L"asm", m_KeywordDefaultColor},
		{L"alignas", m_KeywordDefaultColor},
		{L"alignof", m_KeywordDefaultColor},
		{L"and", m_KeywordDefaultColor},
		{L"and_eq", m_KeywordDefaultColor},
		{L"auto", m_KeywordDefaultColor},
		{L"bitand", m_KeywordDefaultColor},
		{L"bitor", m_KeywordDefaultColor},
		{L"bool", m_KeywordDefaultColor},
		{L"break", m_KeywordSpecialColor},
		{L"case", m_KeywordSpecialColor},
		{L"catch", m_KeywordSpecialColor},
		{L"char", m_KeywordDefaultColor},
		{L"char8_t", m_KeywordDefaultColor},
		{L"char16_t", m_KeywordDefaultColor},
		{L"char32_t", m_KeywordDefaultColor},
		{L"class", m_KeywordDefaultColor},
		{L"compl", m_KeywordDefaultColor},
		{L"const", m_KeywordDefaultColor},
		{L"const_cast", m_KeywordDefaultColor},
		{L"constexpr", m_KeywordDefaultColor},
		{L"continue", m_KeywordSpecialColor },
		{L"decltype", m_KeywordDefaultColor},
		{L"__declspec", m_KeywordDefaultColor},
		{L"dllimport", m_KeywordDefaultColor},
		{L"dllexport", m_KeywordDefaultColor},
		{L"default", m_KeywordSpecialColor },
		{L"delete", m_KeywordDefaultColor},
		{L"do", m_KeywordSpecialColor},
		{L"double", m_KeywordDefaultColor},
		{L"dynamic_cast", m_KeywordDefaultColor},
		{L"else", m_KeywordSpecialColor },
		{L"enum", m_KeywordDefaultColor},
		{L"explicit", m_KeywordDefaultColor},
		{L"extern", m_KeywordDefaultColor},
		{L"false", m_KeywordDefaultColor},
		{L"float", m_KeywordDefaultColor},
		{L"for", m_KeywordSpecialColor},
		{L"friend", m_KeywordDefaultColor},
		{L"final", m_KeywordDefaultColor},
		{L"goto", m_KeywordDefaultColor},
		{L"if", m_KeywordSpecialColor },
		{L"inline", m_KeywordDefaultColor},
		{L"int", m_KeywordDefaultColor},
		{L"long", m_KeywordDefaultColor},
		{L"mutable", m_KeywordDefaultColor},
		{L"namespace", m_KeywordDefaultColor},
		{L"new", m_KeywordDefaultColor},
		{L"noexcept", m_KeywordDefaultColor},
		{L"not", m_KeywordDefaultColor},
		{L"not_eq", m_KeywordDefaultColor},
		{L"nullptr", m_KeywordDefaultColor},
		{L"operator", m_KeywordDefaultColor},
		{L"or", m_KeywordDefaultColor},
		{L"or_eq", m_KeywordDefaultColor},
		{L"private", m_KeywordDefaultColor},
		{L"protected", m_KeywordDefaultColor},
		{L"public", m_KeywordDefaultColor},
		{L"register", m_KeywordDefaultColor},
		{L"reinterpret_cast", m_KeywordDefaultColor},
		{L"return", m_KeywordSpecialColor},
		{L"short", m_KeywordDefaultColor},
		{L"signed", m_KeywordDefaultColor},
		{L"sizeof", m_KeywordDefaultColor},
		{L"static", m_KeywordDefaultColor},
		{L"static_assert", m_KeywordDefaultColor},
		{L"static_cast", m_KeywordDefaultColor},
		{L"struct", m_KeywordDefaultColor},
		{L"switch", m_KeywordSpecialColor },
		{L"template", m_KeywordDefaultColor},
		{L"this", m_KeywordDefaultColor},
		{L"thread_local", m_KeywordDefaultColor},
		{L"throw", m_KeywordSpecialColor },
		{L"true", m_KeywordDefaultColor},
		{L"try", m_KeywordSpecialColor },
		{L"typedef", m_KeywordDefaultColor},
		{L"typeid", m_KeywordDefaultColor},
		{L"union", m_KeywordDefaultColor},
		{L"unsigned", m_KeywordDefaultColor},
		{L"using", m_KeywordDefaultColor},
		{L"virtual", m_KeywordDefaultColor},
		{L"void", m_KeywordDefaultColor},
		{L"volatile", m_KeywordDefaultColor},
		{L"wchar_t", m_KeywordDefaultColor},
		{L"while", m_KeywordSpecialColor }, 
		{L"xor", m_KeywordDefaultColor},
		{L"xor_eq", m_KeywordDefaultColor},
	};
#elif TARGET_LANG == TARGET_LANG_TYPE_PYTHON
	vec4 m_NumberLiteralColor = hex2rgba(0xb5cea8);
	vec4 m_CommentColor = hex2rgba(0x57a64a);
	vec4 m_StringLiteralColor = hex2rgba(0xce9178);
	vec4 m_KeywordDefaultColor = hex2rgba(0xd8a0df);
	vec4 m_KeywordSpecialColor = hex2rgba(0x569cd6);
	vec4 m_MagicMethodColor = hex2rgba(0xFF0000);
	vec4 m_BuiltinFunctionColor = hex2rgba(0xff00ff);
	std::map<std::wstring, vec4> m_Keywords = {
		{L"abs", m_BuiltinFunctionColor},
		{L"aiter", m_BuiltinFunctionColor},
		{L"all", m_BuiltinFunctionColor},
		{L"anext", m_BuiltinFunctionColor},
		{L"any", m_BuiltinFunctionColor},
		{L"ascii", m_BuiltinFunctionColor},
		{L"bin", m_BuiltinFunctionColor},
		{L"bool", m_BuiltinFunctionColor},
		{L"breakpoint", m_BuiltinFunctionColor},
		{L"bytearray", m_BuiltinFunctionColor},
		{L"bytes", m_BuiltinFunctionColor},
		{L"callable", m_BuiltinFunctionColor},
		{L"chr", m_BuiltinFunctionColor},
		{L"classmethod", m_BuiltinFunctionColor},
		{L"compile", m_BuiltinFunctionColor},
		{L"complex", m_BuiltinFunctionColor},
		{L"delattr", m_BuiltinFunctionColor},
		{L"dict", m_BuiltinFunctionColor},
		{L"dir", m_BuiltinFunctionColor},
		{L"divmod", m_BuiltinFunctionColor},
		{L"enumerate", m_BuiltinFunctionColor},
		{L"eval", m_BuiltinFunctionColor},
		{L"exec", m_BuiltinFunctionColor},
		{L"filter", m_BuiltinFunctionColor},
		{L"float", m_BuiltinFunctionColor},
		{L"format", m_BuiltinFunctionColor},
		{L"frozenset", m_BuiltinFunctionColor},
		{L"getattr", m_BuiltinFunctionColor},
		{L"globals", m_BuiltinFunctionColor},
		{L"hasattr", m_BuiltinFunctionColor},
		{L"hash", m_BuiltinFunctionColor},
		{L"help", m_BuiltinFunctionColor},
		{L"hex", m_BuiltinFunctionColor},
		{L"id", m_BuiltinFunctionColor},
		{L"input", m_BuiltinFunctionColor},
		{L"int", m_BuiltinFunctionColor},
		{L"isinstance", m_BuiltinFunctionColor},
		{L"issubclass", m_BuiltinFunctionColor},
		{L"iter", m_BuiltinFunctionColor},
		{L"len", m_BuiltinFunctionColor},
		{L"list", m_BuiltinFunctionColor},
		{L"locals", m_BuiltinFunctionColor},
		{L"map", m_BuiltinFunctionColor},
		{L"max", m_BuiltinFunctionColor},
		{L"memoryview", m_BuiltinFunctionColor},
		{L"min", m_BuiltinFunctionColor},
		{L"next", m_BuiltinFunctionColor},
		{L"object", m_BuiltinFunctionColor},
		{L"oct", m_BuiltinFunctionColor},
		{L"open", m_BuiltinFunctionColor},
		{L"ord", m_BuiltinFunctionColor},
		{L"pow", m_BuiltinFunctionColor},
		{L"print", m_BuiltinFunctionColor},
		{L"property", m_BuiltinFunctionColor},
		{L"range", m_BuiltinFunctionColor},
		{L"repr", m_BuiltinFunctionColor},
		{L"reversed", m_BuiltinFunctionColor},
		{L"round", m_BuiltinFunctionColor},
		{L"set", m_BuiltinFunctionColor},
		{L"setattr", m_BuiltinFunctionColor},
		{L"slice", m_BuiltinFunctionColor},
		{L"sorted", m_BuiltinFunctionColor},
		{L"staticmethod", m_BuiltinFunctionColor},
		{L"str", m_BuiltinFunctionColor},
		{L"sum", m_BuiltinFunctionColor},
		{L"tuple", m_BuiltinFunctionColor},
		{L"type", m_BuiltinFunctionColor},
		{L"vars", m_BuiltinFunctionColor},
		{L"zip", m_BuiltinFunctionColor},
		{L"__import__", m_BuiltinFunctionColor},
		{L"super", m_MagicMethodColor},
		{L"self", m_MagicMethodColor},
		{L"__new__", m_MagicMethodColor},
		{L"__init__", m_MagicMethodColor},
		{L"__del__", m_MagicMethodColor},
		{L"__eq__", m_MagicMethodColor},
		{L"__ne__", m_MagicMethodColor},
		{L"__lt__", m_MagicMethodColor},
		{L"__gt__", m_MagicMethodColor},
		{L"__le__", m_MagicMethodColor},
		{L"__ge__", m_MagicMethodColor},
		{L"__cmp__", m_MagicMethodColor},
		{L"__pos__", m_MagicMethodColor},
		{L"__neg__", m_MagicMethodColor},
		{L"__abs__", m_MagicMethodColor},
		{L"__round__", m_MagicMethodColor},
		{L"__floor__", m_MagicMethodColor},
		{L"__ceil__", m_MagicMethodColor},
		{L"__trunc__", m_MagicMethodColor},
		{L"__invert__", m_MagicMethodColor},
		{L"__index__", m_MagicMethodColor},
		{L"__nonzero__", m_MagicMethodColor},
		{L"__add__", m_MagicMethodColor},
		{L"__sub__", m_MagicMethodColor},
		{L"__mul__", m_MagicMethodColor},
		{L"__floordiv__", m_MagicMethodColor},
		{L"__div__", m_MagicMethodColor},
		{L"__truediv__", m_MagicMethodColor},
		{L"__mod__", m_MagicMethodColor},
		{L"__divmod__", m_MagicMethodColor},
		{L"__pow__", m_MagicMethodColor},
		{L"__lshift__", m_MagicMethodColor},
		{L"__rshift__", m_MagicMethodColor},
		{L"__and__", m_MagicMethodColor},
		{L"__or__", m_MagicMethodColor},
		{L"__xor__", m_MagicMethodColor},
		{L"__radd__", m_MagicMethodColor},
		{L"__rsub__", m_MagicMethodColor},
		{L"__rmul__", m_MagicMethodColor},
		{L"__rfloordiv__", m_MagicMethodColor},
		{L"__rdiv__", m_MagicMethodColor},
		{L"__rtruediv__", m_MagicMethodColor},
		{L"__rmod__", m_MagicMethodColor},
		{L"__rdivmod__", m_MagicMethodColor},
		{L"__rpow__", m_MagicMethodColor},
		{L"__rlshift__", m_MagicMethodColor},
		{L"__rrshift__", m_MagicMethodColor},
		{L"__rand__", m_MagicMethodColor},
		{L"__ror__", m_MagicMethodColor},
		{L"__rxor__", m_MagicMethodColor},
		{L"__iadd__", m_MagicMethodColor},
		{L"__isub__", m_MagicMethodColor},
		{L"__imul__", m_MagicMethodColor},
		{L"__ifloordiv__", m_MagicMethodColor},
		{L"__idiv__", m_MagicMethodColor},
		{L"__itruediv__", m_MagicMethodColor},
		{L"__imod__", m_MagicMethodColor},
		{L"__idivmod__", m_MagicMethodColor},
		{L"__ipow__", m_MagicMethodColor},
		{L"__ilshift__", m_MagicMethodColor},
		{L"__irshift__", m_MagicMethodColor},
		{L"__iand__", m_MagicMethodColor},
		{L"__ior__", m_MagicMethodColor},
		{L"__ixor__", m_MagicMethodColor},
		{L"__int__", m_MagicMethodColor},
		{L"__long__", m_MagicMethodColor},
		{L"__float__", m_MagicMethodColor},
		{L"__complex__", m_MagicMethodColor},
		{L"__oct__", m_MagicMethodColor},
		{L"__hex__", m_MagicMethodColor},
		{L"__index__", m_MagicMethodColor},
		{L"__coerce__", m_MagicMethodColor},
		{L"__getattr__", m_MagicMethodColor},
		{L"__setattr__", m_MagicMethodColor},
		{L"__delattr__", m_MagicMethodColor},
		{L"__getattribute__", m_MagicMethodColor},
		{L"__getitem__", m_MagicMethodColor},
		{L"__setitem__", m_MagicMethodColor},
		{L"__delitem__", m_MagicMethodColor},
		{L"__iter__", m_MagicMethodColor},
		{L"__contains__", m_MagicMethodColor},
		{L"__call__", m_MagicMethodColor},
		{L"__enter__", m_MagicMethodColor},
		{L"__exit__", m_MagicMethodColor},
		{L"__getstate__", m_MagicMethodColor},
		{L"__setstate__", m_MagicMethodColor},
		{L"__str__", m_MagicMethodColor},
		{L"__repr__", m_MagicMethodColor},
		{L"__unicode__", m_MagicMethodColor},
		{L"__format__", m_MagicMethodColor},
		{L"__hash__", m_MagicMethodColor},
		{L"__dir__", m_MagicMethodColor},
		{L"__sizeof__", m_MagicMethodColor},
		{L"__len__", m_MagicMethodColor},
		{L"__reversed__", m_MagicMethodColor},
		{L"__contains__", m_MagicMethodColor},
		{L"__missing__", m_MagicMethodColor},
		{L"__copy__", m_MagicMethodColor},
		{L"__deepcopy__", m_MagicMethodColor},
		{L"__getinitargs__", m_MagicMethodColor},
		{L"__getnewargs__", m_MagicMethodColor},
		{L"__reduce__", m_MagicMethodColor},
		{L"__reduce_ex__", m_MagicMethodColor},
		{L"False", m_KeywordSpecialColor},
		{L"await", m_KeywordDefaultColor},
		{L"else", m_KeywordDefaultColor},
		{L"import", m_KeywordDefaultColor},
		{L"pass", m_KeywordDefaultColor},
		{L"None", m_KeywordSpecialColor},
		{L"break", m_KeywordDefaultColor},
		{L"except", m_KeywordDefaultColor},
		{L"in", m_KeywordSpecialColor},
		{L"raise", m_KeywordDefaultColor},
		{L"True", m_KeywordSpecialColor},
		{L"class", m_KeywordSpecialColor},
		{L"finally", m_KeywordDefaultColor},
		{L"is", m_KeywordSpecialColor},
		{L"return", m_KeywordDefaultColor},
		{L"and", m_KeywordSpecialColor},
		{L"continue", m_KeywordDefaultColor},
		{L"for", m_KeywordDefaultColor},
		{L"lambda", m_KeywordSpecialColor},
		{L"try", m_KeywordDefaultColor},
		{L"as", m_KeywordDefaultColor},
		{L"def", m_KeywordDefaultColor},
		{L"from", m_KeywordDefaultColor},
		{L"nonlocal", m_KeywordDefaultColor},
		{L"while", m_KeywordDefaultColor},
		{L"assert", m_KeywordDefaultColor},
		{L"del", m_KeywordDefaultColor},
		{L"global", m_KeywordDefaultColor},
		{L"not", m_KeywordSpecialColor},
		{L"with", m_KeywordDefaultColor},
		{L"async", m_KeywordDefaultColor},
		{L"elif", m_KeywordDefaultColor},
		{L"if", m_KeywordDefaultColor},
		{L"or", m_KeywordSpecialColor},
		{L"yield", m_KeywordDefaultColor},
	};
#elif TARGET_LANG == TARGET_LANG_TYPE_JAVASCRIPT
	vec4 m_NumberLiteralColor = hex2rgba(0xb5cea8);
	vec4 m_CommentColor = hex2rgba(0x57a64a);
	vec4 m_StringLiteralColor = hex2rgba(0xce9178);
	vec4 m_KeywordDefaultColor = hex2rgba(0xd8a0df);
	vec4 m_KeywordSpecialColor = hex2rgba(0x569cd6);
	vec4 m_FunctionColor = hex2rgba(0xd4b964);
	vec4 m_DatatypeColor = hex2rgba(0x4ec9b0);
	std::map<std::wstring, vec4> m_Keywords = {
		{L"Any", m_DatatypeColor},
		{L"ArrayBuffer", m_DatatypeColor},
		{L"Array", m_DatatypeColor},
		{L"Boolean", m_DatatypeColor},
		{L"Constant", m_DatatypeColor},
		{L"Float", m_DatatypeColor},
		{L"Function", m_DatatypeColor},
		{L"HTMLElement", m_DatatypeColor},
		{L"Integer", m_DatatypeColor},
		{L"null", m_DatatypeColor},
		{L"Object", m_DatatypeColor},
		{L"String", m_DatatypeColor},
		{L"Float32Array", m_DatatypeColor},
		{L"Uint8Array", m_DatatypeColor},
		{L"Int8Array", m_DatatypeColor},
		{L"Uint16Array", m_DatatypeColor},
		{L"Int16Array", m_DatatypeColor},
		{L"Uint32Array", m_DatatypeColor},
		{L"Int32Array", m_DatatypeColor},
		{L"undefined", m_DatatypeColor},
		{L"void", m_DatatypeColor},
		{L"never", m_DatatypeColor},
		{L"while", m_KeywordSpecialColor},
		{L"case", m_KeywordSpecialColor},
		{L"await", m_KeywordSpecialColor},
		{L"class", m_KeywordDefaultColor},
		{L"void", m_KeywordDefaultColor},
		{L"function", m_KeywordDefaultColor},
		{L"instanceof", m_KeywordDefaultColor},
		{L"throw", m_KeywordSpecialColor},
		{L"export", m_KeywordDefaultColor},
		{L"delete", m_KeywordDefaultColor},
		{L"catch", m_KeywordSpecialColor},
		{L"private", m_KeywordDefaultColor},
		{L"package", m_KeywordDefaultColor},
		{L"true", m_KeywordDefaultColor},
		{L"debugger", m_KeywordDefaultColor},
		{L"extends", m_KeywordDefaultColor},
		{L"default", m_KeywordSpecialColor},
		{L"interface", m_KeywordDefaultColor},
		{L"super", m_KeywordDefaultColor},
		{L"with", m_KeywordDefaultColor},
		{L"enum", m_KeywordDefaultColor},
		{L"if", m_KeywordSpecialColor},
		{L"return", m_KeywordSpecialColor},
		{L"switch", m_KeywordSpecialColor},
		{L"try", m_KeywordSpecialColor},
		{L"let", m_KeywordDefaultColor},
		{L"yield", m_KeywordSpecialColor},
		{L"typeof", m_KeywordDefaultColor},
		{L"public", m_KeywordDefaultColor},
		{L"for", m_KeywordSpecialColor},
		{L"static", m_KeywordDefaultColor},
		{L"new", m_KeywordDefaultColor},
		{L"else", m_KeywordSpecialColor},
		{L"finally", m_KeywordDefaultColor},
		{L"false", m_KeywordDefaultColor},
		{L"import", m_KeywordDefaultColor},
		{L"var", m_KeywordDefaultColor},
		{L"do", m_KeywordSpecialColor},
		{L"protected", m_KeywordDefaultColor},
		{L"in", m_KeywordDefaultColor},
		{L"implements", m_KeywordDefaultColor},
		{L"this", m_KeywordDefaultColor},
		{L"const", m_KeywordDefaultColor},
		{L"continue", m_KeywordSpecialColor},
		{L"break", m_KeywordSpecialColor},
	};
#endif
private:
	class Shader* m_shader = nullptr;
	class Camera* m_camera = nullptr;
	std::vector<std::optional<class GlyphTexture*>> m_glyphTextures{};
	std::map<wchar_t, class GlyphTexture*> m_glyphTextureMap{};
	unsigned int m_VAO, m_VBO;
private:
	bool m_enableRainbow = false;
	std::vector<SyntaxHighlight> m_higilightList{};
	vec2 m_position = vec2();
	vec4 m_color = vec4(1, 1, 1, 1);
	vec4 m_selectionColor = vec4(0.45, 0.45, 0.45, 0.6);
	vec2 m_size = vec2(0, 0);
	std::wstring m_text;
	size_t m_escapeSequenceCount = 0;
	std::vector<std::pair<int, int>> m_blockList{};
	std::vector<class ColorRect*> m_selectionList{};
private:
	void updateHighlight();
	void updateBlockList();
public:
	Label(class Camera* _cam, std::wstring _text);
	~Label();
	void update();
	void draw() const;
public:
	void setPosition(const vec2& _pos);
	const vec2& getPosition();
	const vec2& getSize();
	const std::vector<std::optional<class GlyphTexture*>>& getGlyphTextures();
	const size_t& getEscapeSequenceCount();
	int getBelongBlock(const int& at);
	int getLongestBlock();
	const std::vector<std::pair<int, int>>& getBlockList();
	const std::wstring& getText();
	void updateSelection(const size_t& from, const size_t& to);
	void addSelectionSection();
	void eraseSelectionSection(const int& at);
	void toggleRainbow();
public:
	void push_back(const wchar_t& ch);
	void insert(const size_t& at, const wchar_t& ch);
	void pop_back();
	void erase(const size_t& at);
};







#endif