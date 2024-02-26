#include "editor.h"

Editor* myEditor = nullptr;

int main(const int argc, const char* argv[]) {
	myEditor = new Editor((argc > 1 ? argv[1] : ""));
	myEditor->run();
	delete myEditor;
	return EXIT_SUCCESS;
}