#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <iterator>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "imgui.h"


//
//	TextEditor
//

class TextEditor {
public:
	// constructor
	TextEditor() { SetPalette(defaultPalette); }

	//
	// Below is the public API
	// Public member functions start with an uppercase character to be consistent with Dear ImGui
	//

	// access editor options
	void SetTabSize(int value) {
		// this must be called before text is loaded/edited
		if (document.isEmpty() && transactions.empty()) {
			document.setTabSize(std::max(1, std::min(8, value)));
		}
	}

	int GetTabSize() const { return document.getTabSize(); }
	void SetLineSpacing(float value) { lineSpacing = std::max(1.0f, std::min(2.0f, value)); }
	float GetLineSpacing() const { return lineSpacing; }
	void SetReadOnlyEnabled(bool value) { readOnly = value; }
	bool IsReadOnlyEnabled() const { return readOnly; }
	void SetAutoIndentEnabled(bool value) { autoIndent = value; }
	bool IsAutoIndentEnabled() const { return autoIndent; }
	void SetShowWhitespacesEnabled(bool value) { showWhitespaces = value; }
	bool IsShowWhitespacesEnabled() const { return showWhitespaces; }
	void SetShowLineNumbersEnabled(bool value) { showLineNumbers = value; }
	bool IsShowLineNumbersEnabled() const { return showLineNumbers; }
	void SetShowScrollbarMiniMapEnabled(bool value) { showScrollbarMiniMap = value; }
	bool IsShowScrollbarMiniMapEnabled() const { return showScrollbarMiniMap; }
	void SetShowPanScrollIndicatorEnabled(bool value) { showPanScrollIndicator = value; }
	bool IsShowPanScrollIndicatorEnabled() const { return showPanScrollIndicator; }
	void SetShowMatchingBrackets(bool value) { showMatchingBrackets = value; showMatchingBracketsChanged = true; }
	bool IsShowingMatchingBrackets() const { return showMatchingBrackets; }
	void SetCompletePairedGlyphs(bool value) { completePairedGlyphs = value; }
	bool IsCompletingPairedGlyphs() const { return completePairedGlyphs; }
	void SetOverwriteEnabled(bool value) { overwrite = value; }
	bool IsOverwriteEnabled() const { return overwrite; }
	void SetMiddleMousePanMode() { panMode = true; }
	void SetMiddleMouseScrollMode() { panMode = false; }
	bool IsMiddleMousePanMode() const { return panMode; }

	// access text (using UTF-8 encoded strings)
	// (see note below on cursor and scroll manipulation after setting new text)
	void SetText(const std::string& text) { setText(text); }
	std::string GetText() const { return document.getText(); }
	std::string GetCursorText(size_t cursor) const { return getCursorText(cursor); }

	std::string GetLineText(int line) const {
		return (line < 0 || line > static_cast<int>(document.size())) ? "" : document.getLineText(line);
	}

	std::string GetSectionText(int startLine, int startColumn, int endLine, int endColumn) const {
		return document.getSectionText(
			document.normalizeCoordinate(Coordinate(startLine, startColumn)),
			document.normalizeCoordinate(Coordinate(endLine, endColumn)));
	}

	void ClearText() { SetText(""); }

	bool IsEmpty() const { return document.isEmpty(); }
	int GetLineCount() const { return document.lineCount(); }

	// render the text editor in a Dear ImGui context
	void Render(const char* title, const ImVec2& size=ImVec2(), bool border=false) { render(title, size, border); }

	// programmatically set focus on the editor
	void SetFocus() { focusOnEditor = true; }

	// clipboard actions
	void Cut() { if (!readOnly) cut(); }
	void Copy() const { copy(); }
	void Paste() { if (!readOnly) paste(); }
	void Undo() { if (!readOnly) undo(); }
	void Redo() { if (!readOnly) redo(); }
	bool CanUndo() const { return !readOnly && transactions.canUndo(); };
	bool CanRedo() const { return !readOnly && transactions.canRedo(); };
	size_t GetUndoIndex() const { return transactions.getUndoIndex(); };

	// manipulate cursors and selections (line numbers are zero-based)
	void SetCursor(int line, int column) { moveTo(document.normalizeCoordinate(Coordinate(line, column)), false); }
	void SelectAll() { selectAll(); }
	void SelectLine(int line) { if (line >= 0 && line < document.lineCount()) selectLine(line); }
	void SelectLines(int start, int end) { if (start >= 0 && end < document.lineCount() && start <= end) selectLines(start, end); }

	void SelectRegion(int startLine, int startColumn, int endLine, int endColumn) {
		selectRegion(startLine, startColumn, endLine, endColumn);
	}

	void SelectToBrackets(bool includeBrackets=true) { selectToBrackets(includeBrackets); }
	void GrowSelectionsToCurlyBrackets() { growSelectionsToCurlyBrackets(); }
	void ShrinkSelectionsToCurlyBrackets() { shrinkSelectionsToCurlyBrackets(); }
	void AddNextOccurrence() { addNextOccurrence(); }
	void SelectAllOccurrences() { selectAllOccurrences(); }
	bool AnyCursorHasSelection() const { return cursors.anyHasSelection(); }
	bool AllCursorsHaveSelection() const { return cursors.allHaveSelection(); }
	bool CurrentCursorHasSelection() const { return cursors.currentCursorHasSelection(); }
	void ClearCursors() { cursors.clearAll(); }

	// get cursor positions (the meaning of main and current is explained in README.md)
	size_t GetNumberOfCursors() const { return cursors.size(); }
	void GetCursor(int& line, int& column, size_t cursor) const { return getCursor(line, column, cursor); }
	void GetCursor(int& startLine, int& startColumn, int& endLine, int& endColumn, size_t cursor) const { return getCursor(startLine, startColumn, endLine, endColumn, cursor); }
	void GetMainCursor(int& line, int& column) const { return getCursor(line, column, cursors.getMainIndex()); }
	void GetCurrentCursor(int& line, int& column) const { return getCursor(line, column, cursors.getCurrentIndex()); }

	// scrolling support
	enum class Scroll {
		alignTop,
		alignMiddle,
		alignBottom
	};

	void ScrollToLine(int line, Scroll alignment) { scrollToLine(line, alignment); }
	int GetFirstVisibleLine() const { return firstVisibleLine; }
	int GetLastVisibleLine() const { return lastVisibleLine; }
	int GetFirstVisibleColumn() const { return firstVisibleColumn; }
	int GetLastVisibleColumn() const { return lastVisibleColumn; }

	float GetLineHeight() const { return glyphSize.y; }
	float GetGlyphWidth() const { return glyphSize.x; }

	// note on setting cursor and scrolling
	//
	// calling SetCursor or ScrollToLine has no effect until the next call to Render
	// this is because we can only do layout calculations when we are in a Dear ImGui drawing context
	// as a result, SetCursor or ScrollToLine just mark the request and let Render execute it
	//
	// the order of the calls is therefore important as they can interfere with each other
	// so if you call SetText, SetCursor and/or ScrollToLine before Render, the order should be:
	//
	// * call SetText first as it resets the entire editor state including cursors and scrolling
	// * then call SetCursor as it sets the cursor and requests that we make the cursor visible (i.e. scroll to it)
	// * then call ScrollToLine to mark the exact scroll location (it cancels the possible SetCursor scroll request)
	// * call Render to properly update the entire state
	//
	// this works on opening the editor as well as later

	// find/replace support
	void SelectFirstOccurrenceOf(const std::string& text, bool caseSensitive=true, bool wholeWord=false) { selectFirstOccurrenceOf(text, caseSensitive, wholeWord); }
	void SelectNextOccurrenceOf(const std::string& text, bool caseSensitive=true, bool wholeWord=false) { selectNextOccurrenceOf(text, caseSensitive, wholeWord); }
	void SelectAllOccurrencesOf(const std::string& text, bool caseSensitive=true, bool wholeWord=false) { selectAllOccurrencesOf(text, caseSensitive, wholeWord); }
	void ReplaceTextInCurrentCursor(const std::string& text) { if (!readOnly) replaceTextInCurrentCursor(text); }
	void ReplaceTextInAllCursors(const std::string& text) { if (!readOnly) replaceTextInAllCursors(text); }

	void OpenFindReplaceWindow() { findReplaceVisible = true; focusOnFind = true; }
	void SetFindButtonLabel(const std::string& label) { findButtonLabel = label; }
	void SetFindAllButtonLabel(const std::string& label) { findAllButtonLabel = label; }
	void SetReplaceButtonLabel(const std::string& label) { replaceButtonLabel = label; }
	void SetReplaceAllButtonLabel(const std::string& label) { replaceAllButtonLabel = label; }
	bool HasFindString() const { return findText.size(); }
	void FindNext() { findNext(); }
	void FindAll() { findAll(); }

	// access markers (line numbers are zero-based)
	void AddMarker(int line, ImU32 lineNumberColor, ImU32 textColor, const std::string& lineNumberTooltip, const std::string& textTooltip) { addMarker(line, lineNumberColor, textColor, lineNumberTooltip, textTooltip); }
	void ClearMarkers() { clearMarkers(); }
	bool HasMarkers() const { return markers.size() != 0; }

	// line-based decoration
	struct Decorator {
		int line; // zero-based
		float width;
		float height;
		ImVec2 glyphSize;
	};

	// positive width is number of pixels, negative with is number of glyphs
	void SetLineDecorator(float width, std::function<void(Decorator& decorator)> callback) {
		decoratorWidth = width;
		decoratorCallback = callback;
	}

	void ClearLineDecorator() { SetLineDecorator(0.0f, nullptr); }
	bool HasLineDecorator() const { return decoratorWidth != 0.0f && decoratorCallback != nullptr; }

	// setup context menu callbacks (these are called when a user right clicks line numbers or somewhere in the text)
	// the editor sets up the popup menus, the callback has to populate them
	void SetLineNumberContextMenuCallback(std::function<void(int line)> callback) { lineNumberContextMenuCallback = callback; }
	void ClearLineNumberContextMenuCallback() { SetLineNumberContextMenuCallback(nullptr); }
	bool HasLineNumberContextMenuCallback() const { return lineNumberContextMenuCallback != nullptr; }

	void SetTextContextMenuCallback(std::function<void(int line, int column)> callback) { textContextMenuCallback = callback; }
	void ClearTextContextMenuCallback() { SetTextContextMenuCallback(nullptr); }
	bool HasTextContextMenuCallback() const { return textContextMenuCallback != nullptr; }

	// useful functions to work on selections
	void IndentLines() { if (!readOnly) indentLines(); }
	void DeindentLines() { if (!readOnly) deindentLines(); }
	void MoveUpLines() { if (!readOnly) moveUpLines(); }
	void MoveDownLines() { if (!readOnly) moveDownLines(); }
	void ToggleComments() { if (!readOnly && language) toggleComments(); }
	void FilterSelections(std::function<std::string(std::string)> filter) { if (!readOnly) filterSelections(filter); }
	void SelectionToLowerCase() { if (!readOnly) selectionToLowerCase(); }
	void SelectionToUpperCase() { if (!readOnly) selectionToUpperCase(); }

	// useful functions to work on entire text
	void StripTrailingWhitespaces() { if (!readOnly) stripTrailingWhitespaces(); }
	void FilterLines(std::function<std::string(std::string)> filter) { if (!readOnly) filterLines(filter); }
	void TabsToSpaces() { if (!readOnly) tabsToSpaces(); }
	void SpacesToTabs() { if (!readOnly) spacesToTabs(); }

	// NOTE: functions provided to FilterSelections or FilterLines
	//       should accept and return UTF-8 encoded strings

	// color palette support
	enum class Color : char {
		text,
		keyword,
		declaration,
		number,
		string,
		punctuation,
		preprocessor,
		identifier,
		knownIdentifier,
		comment,
		background,
		cursor,
		selection,
		whitespace,
		matchingBracketBackground,
		matchingBracketActive,
		matchingBracketLevel1,
		matchingBracketLevel2,
		matchingBracketLevel3,
		matchingBracketError,
		lineNumber,
		currentLineNumber,
		count
	};

	class Palette : public std::array<ImU32, static_cast<size_t>(Color::count)> {
	public:
		ImU32 get(Color color) const { return at(static_cast<size_t>(color)); }
	};

	void SetPalette(const Palette& newPalette) { paletteBase = newPalette; paletteAlpha = -1.0f; }
	const Palette& GetPalette() const { return paletteBase; }
	static void SetDefaultPalette(const Palette& aValue) { defaultPalette = aValue; }
	static Palette& GetDefaultPalette() { return defaultPalette; }

	static const Palette& GetDarkPalette();
	static const Palette& GetLightPalette();

	// a single colored character (a glyph)
	class Glyph {
	public:
		// constructors
		Glyph() = default;
		Glyph(ImWchar cp) : codepoint(cp) {}
		Glyph(ImWchar cp, Color col) : codepoint(cp), color(col) {}

		// properties
		ImWchar codepoint = 0;
		Color color = Color::text;
	};

	// iterator used in language specific tokenizers
	class Iterator {
	public:
		// constructors
		Iterator() = default;
		Iterator(Glyph* g) : glyph(g) {}

		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = ImWchar;
		using pointer = ImWchar*;
		using reference = ImWchar&;

		reference operator*() const { return glyph->codepoint; }
		pointer operator->() const { return &(glyph->codepoint); }
		Iterator& operator++() { glyph++; return *this; }
		Iterator operator++(int) { Iterator tmp = *this; glyph++; return tmp; }
		size_t operator-(const Iterator& a) { return glyph - a.glyph; }
		friend bool operator== (const Iterator& a, const Iterator& b) { return a.glyph == b.glyph; };
		friend bool operator!= (const Iterator& a, const Iterator& b) { return !(a.glyph == b.glyph); };
		friend bool operator< (const Iterator& a, const Iterator& b) { return a.glyph < b.glyph; };
		friend bool operator<= (const Iterator& a, const Iterator& b) { return a.glyph <= b.glyph; };
		friend bool operator> (const Iterator& a, const Iterator& b) { return a.glyph > b.glyph; };
		friend bool operator>= (const Iterator& a, const Iterator& b) { return a.glyph >= b.glyph; };

	private:
		// properties
		Glyph* glyph;
	};

	// language support
	class Language {
	public:
		// name of the language
		std::string name;

		// flag to describe if keywords and identifiers are case sensitive (which is the default)
		bool caseSensitive = true;

		// the character that starts a preprocessor directive (can be 0 if language doesn't have this feature)
		ImWchar preprocess = 0;

		// a character sequence that start a single line comment (can be blank if language doesn't have this feature)
		std::string singleLineComment;

		// an alternate single line comment character sequence (can be blank if language doesn't have this feature)
		std::string singleLineCommentAlt;

		// the start and end character sequence for multiline comments (can be blank language doesn't have this feature)
		std::string commentStart;
		std::string commentEnd;

		// flags specifying whether language supports single quoted ['] and/or double quoted [""] strings
		bool hasSingleQuotedStrings = false;
		bool hasDoubleQuotedStrings = false;

		// other character sequences that starts and ends strings (can be blank if language doesn't have this feature)
		std::string otherStringStart;
		std::string otherStringEnd;

		// alternate character sequences that starts and ends strings (can be blank if language doesn't have this feature)
		std::string otherStringAltStart;
		std::string otherStringAltEnd;

		// character inside string used to escape the next character (can be 0 if language doesn't have this feature)
		ImWchar stringEscape = 0;

		// set of keywords, declarations, identifiers used in the language (can be blank if language doesn't have these features)
		// if language is not case sensitive, all entries should be in lower case
		std::unordered_set<std::string> keywords;
		std::unordered_set<std::string> declarations;
		std::unordered_set<std::string> identifiers;

		// function to determine if specified character in considered punctuation
		std::function<bool(ImWchar)> isPunctuation;

		// functions to tokenize identifiers and numbers (can be nullptr if language doesn't have this feature)
		// start and end refer to the characters being tokonized
		// functions should return the an iterator to the character after the token
		//		returning start means no token was found
		std::function<Iterator(Iterator start, Iterator end)> getIdentifier;
		std::function<Iterator(Iterator start, Iterator end)> getNumber;

		// function to implement custom tokonizer
		// if a token is found function should return the an iterator to the character after the token
		// and set the color
		std::function<Iterator(Iterator start, Iterator end, Color& color)> customTokenizer;

		// predefined language definitions
		static const Language* C();
		static const Language* Cpp();
		static const Language* Cs();
		static const Language* AngelScript();
		static const Language* Lua();
		static const Language* Python();
		static const Language* Glsl();
		static const Language* Hlsl();
		static const Language* Json();
		static const Language* Markdown();
		static const Language* Sql();
	};

	void SetLanguage(const Language* l) { language = l; languageChanged = true; }
	const Language* GetLanguage() const { return language; };
	bool HasLanguage() const { return language != nullptr; }
	std::string GetLanguageName() const { return language == nullptr ? "None" : language->name; }

	// support functions for unicode codepoints
	class CodePoint {
	public:
		static std::string::const_iterator skipBOM(std::string::const_iterator i, std::string::const_iterator end);
		static std::string::const_iterator read(std::string::const_iterator i, std::string::const_iterator end, ImWchar* codepoint);
		static size_t write(char* i, ImWchar codepoint); // must point to buffer of 4 characters (returns number of characters written)
		static bool isLetter(ImWchar codepoint);
		static bool isNumber(ImWchar codepoint);
		static bool isWord(ImWchar codepoint);
		static bool isWhiteSpace(ImWchar codepoint);
		static bool isXidStart(ImWchar codepoint);
		static bool isXidContinue(ImWchar codepoint);
		static bool isLower(ImWchar codepoint);
		static bool isUpper(ImWchar codepoint);
		static ImWchar toUpper(ImWchar codepoint);
		static ImWchar toLower(ImWchar codepoint);

		static constexpr ImWchar singleQuote = '\'';
		static constexpr ImWchar doubleQuote = '"';
		static constexpr ImWchar openCurlyBracket = '{';
		static constexpr ImWchar closeCurlyBracket = '}';
		static constexpr ImWchar openSquareBracket = '[';
		static constexpr ImWchar closeSquareBracket = ']';
		static constexpr ImWchar openParenthesis = '(';
		static constexpr ImWchar closeParenthesis = ')';

		static bool isPairOpener(ImWchar ch) {
			return
				ch == openCurlyBracket ||
				ch == openSquareBracket ||
				ch == openParenthesis ||
				ch == singleQuote ||
				ch == doubleQuote;
		}

		static bool isPairCloser(ImWchar ch) {
			return
				ch == closeCurlyBracket ||
				ch == closeSquareBracket ||
				ch == closeParenthesis ||
				ch == singleQuote ||
				ch == doubleQuote;
		}

		static ImWchar toPairCloser(ImWchar ch) {
			return
				(ch == openCurlyBracket) ? closeCurlyBracket :
				(ch == openSquareBracket) ? closeSquareBracket :
				(ch == openParenthesis) ? closeParenthesis:
				ch;
		}

		static ImWchar toPairOpener(ImWchar ch) {
			return
				(ch == closeCurlyBracket) ? openCurlyBracket :
				(ch == closeSquareBracket) ? openSquareBracket :
				(ch == closeParenthesis) ? openParenthesis:
				ch;
		}

		static bool isMatchingPair(ImWchar open, ImWchar close) {
			return isPairOpener(open) && close == toPairCloser(open);
		}

		static bool isBracketOpener(ImWchar ch) {
			return
				ch == openCurlyBracket ||
				ch == openSquareBracket ||
				ch == openParenthesis;
		}

		static bool isBracketCloser(ImWchar ch) {
			return
				ch == closeCurlyBracket ||
				ch == closeSquareBracket ||
				ch == closeParenthesis;
		}

		static bool isMatchingBrackets(ImWchar open, ImWchar close) {
			return isBracketOpener(open) && close == toPairCloser(open);
		}
	};

protected:
	//
	// below is the private API
	// private members (function and variables) start with a lowercase character
	// private class names start with a lowercase character
	//

	class Coordinate {
		// represent a character coordinate from the user's point of view, i. e. consider an uniform grid
		// on the screen as it is rendered, and each cell has its own coordinate, starting from 0
		//
		// tabs are counted as [1..tabsize] count spaces, depending on how many spaces are necessary to
		// reach the next tab stop
		//
		// for example, coordinate (1, 5) represents the character 'B' in a line "\tABC", when tabsize = 4,
		// because it is rendered as "    ABC" on the screen

	public:
		Coordinate() = default;
		Coordinate(int l, int c) : line(l), column(c) {}

		bool operator ==(const Coordinate& o) const { return line == o.line && column == o.column; }
		bool operator !=(const Coordinate& o) const { return line != o.line || column != o.column; }
		bool operator <(const Coordinate& o) const { return line != o.line ? line < o.line : column < o.column; }
		bool operator >(const Coordinate& o) const { return line != o.line ? line > o.line : column > o.column; }
		bool operator <=(const Coordinate& o) const { return line != o.line ? line < o.line : column <= o.column; }
		bool operator >=(const Coordinate& o) const { return line != o.line ? line > o.line : column >= o.column; }

		Coordinate operator -(const Coordinate& o) const { return Coordinate(line - o.line, column - o.column); }
		Coordinate operator +(const Coordinate& o) const { return Coordinate(line + o.line, column + o.column); }

		static Coordinate invalid() { static Coordinate invalid(-1, -1); return invalid; }
		bool isValid() const { return line >= 0 && column >= 0; }

		int line = 0;
		int column = 0;
	};

	// a single cursor
	class Cursor {
	public:
		// constructors
		Cursor() = default;
		Cursor(Coordinate coordinate) : start(coordinate), end(coordinate) {}
		Cursor(Coordinate s, Coordinate e) : start(s), end(e) {}

		// update the cursor
		void update(Coordinate coordinate) { end = coordinate; updated = true; }
		void update(Coordinate s, Coordinate e) { start = s; end = e; updated = true; }
		void update(Coordinate coordinate, bool keep) { if (keep) update(coordinate); else update(coordinate, coordinate); updated = true; }

		// adjust cursor for insert/delete operations
		// (these functions assume that insert or delete points are before the cursor)
		void adjustForInsert(Coordinate insertStart, Coordinate insertEnd);
		void adjustForDelete(Coordinate deleteStart, Coordinate deleteEnd);

		// access cursor properties
		Coordinate getInteractiveStart() const { return start; }
		Coordinate getInteractiveEnd() const { return end; }
		Coordinate getSelectionStart() const { return start < end ? start : end; }
		Coordinate getSelectionEnd() const { return start > end ? start : end; }
		bool hasSelection() const { return start != end; }

		void resetToStart() { update(getSelectionStart(), getSelectionStart()); }
		void resetToEnd() { update(getSelectionEnd(), getSelectionEnd()); }

		void setMain(bool value) { main = value; }
		bool isMain() const { return main; }

		void setCurrent(bool value) { current = value; }
		bool isCurrent() const { return current; }

		void setUpdated(bool value) { updated = value; }
		bool isUpdated() const { return updated; }

	private:
		// helper functions
		Coordinate adjustCoordinateForInsert(Coordinate coordinate, Coordinate insertStart, Coordinate insertEnd);
		Coordinate adjustCoordinateForDelete(Coordinate coordinate, Coordinate deleteStart, Coordinate deleteEnd);

		// properties
		Coordinate start{0, 0};
		Coordinate end{0, 0};
		bool main = false;
		bool current = true;
		bool updated = true;
	};

	// the current list of cursors
	class Cursors : public std::vector<Cursor> {
	public:
		// constructor
		Cursors() { clearAll(); }

		// reset the cursors
		void reset();

		// erase all cursors and specify a new one
		void setCursor(Coordinate coordinate) { setCursor(coordinate, coordinate); }
		void setCursor(Coordinate start, Coordinate end);

		// add a cursor to the list
		void addCursor(Coordinate c) { addCursor(c, c); }
		void addCursor(Coordinate cursorStart, Coordinate cursorEnd);

		// update the current cursor (the one last added)
		void updateCurrentCursor(Coordinate coordinate) { at(current).update(coordinate); }
		void updateCurrentCursor(Coordinate start, Coordinate end) { at(current).update(start, end); }
		void updateCurrentCursor(Coordinate coordinate, bool keep) { at(current).update(coordinate, keep); }

		// check cursor status
		bool hasMultiple() const { return size() > 1; }
		bool anyHasSelection() const;
		bool allHaveSelection() const;
		bool mainCursorHasSelection() const { return at(main).hasSelection(); }
		bool currentCursorHasSelection() const { return at(current).hasSelection(); }
		bool mainHasUpdate() const { return at(main).isUpdated(); }
		bool anyHasUpdate() const;

		// clear the selections and create the default cursor
		void clearAll();

		// clear all additional cursors
		void clearAdditional(bool reset=false);

		// clear all updated flags
		void clearUpdated();

		// get main/current cursor
		Cursor& getMain() { return at(main); }
		size_t getMainIndex() const { return main; }
		Cursor& getCurrent() { return at(current); }
		size_t getCurrentIndex() const { return current; }
		iterator getCurrentAsIterator() { return begin() + current; }

		// update cursors
		void update();

		// adjust cursors for insert/delete operations
		// (these functions assume that insert or delete points are before the cursor)
		void adjustForInsert(iterator start, Coordinate insertStart, Coordinate insertEnd);
		void adjustForDelete(iterator start, Coordinate deleteStart, Coordinate deleteEnd);

	private:
		size_t main = 0;
		size_t current = 0;
	} cursors;

	// the list of text markers
	class Marker {
	public:
		Marker(ImU32 lc, ImU32 tc, const std::string& lt, const std::string& tt) :
			lineNumberColor(lc), textColor(tc), lineNumberTooltip(lt), textTooltip(tt) {}

		ImU32 lineNumberColor;
		ImU32 textColor;
		std::string lineNumberTooltip;
		std::string textTooltip;
	};

	std::vector<Marker> markers;

	// tokenizer state
	enum class State : char {
		inText,
		inComment,
		inSingleQuotedString,
		inDoubleQuotedString,
		inOtherString,
		inOtherStringAlt
	};

	// a single line in a document
	class Line : public std::vector<Glyph> {
	public:
		// state at start of line
		State state = State::inText;

		// marker reference (0 means no marker for this line)
		size_t marker = 0;

		// width of this line (in visible columns)
		int maxColumn = 0;

		// do we need to (re)colorize this line
		bool colorize = true;
	};

	// the document being edited (Lines of Glyphs)
	class Document : public std::vector<Line> {
	public:
		// constructor
		Document() { emplace_back(); }

		// access document's tab size
		void setTabSize(int ts) { tabSize = ts; }
		int getTabSize() const { return tabSize; }

		// manipulate document text (strings should be UTF-8 encoded)
		void setText(const std::string& text);
		void setText(const std::vector<std::string>& text);
		Coordinate insertText(Coordinate start, const std::string& text);
		void deleteText(Coordinate start, Coordinate end);

		// access document text (strings are UTF-8 encoded)
		std::string getText() const;
		std::string getLineText(int line) const;
		std::string getSectionText(Coordinate start, Coordinate end) const;
		ImWchar getCodePoint(Coordinate location);

		// see if document is empty
		bool isEmpty() const { return size() == 1 && at(0).size() == 0; }

		// get number of lines (as an int)
		int lineCount() const { return static_cast<int>(size()); }

		// update maximum column numbers for this document and the specified lines
		void updateMaximumColumn(int first, int last);
		int getMaxColumn() const { return maxColumn; }

		// translate visible column to line index (and visa versa)
		size_t getIndex(const Line& line, int column) const;
		size_t getIndex(Coordinate coordinate) const { return getIndex(at(coordinate.line), coordinate.column); }
		int getColumn(const Line& line, size_t index) const;
		int getColumn(int line, size_t index) const { return getColumn(at(line), index); }

		// coordinate operations in context of document
		Coordinate getUp(Coordinate from, int lines=1) const;
		Coordinate getDown(Coordinate from, int lines=1) const;
		Coordinate getLeft(Coordinate from, bool wordMode=false) const;
		Coordinate getRight(Coordinate from, bool wordMode=false) const;
		Coordinate getTop() const;
		Coordinate getBottom() const;
		Coordinate getStartOfLine(Coordinate from) const;
		Coordinate getEndOfLine(Coordinate from) const;
		Coordinate getNextLine(Coordinate from) const { return getRight(getEndOfLine(from)); }

		// search in document
		Coordinate findWordStart(Coordinate from) const;
		Coordinate findWordEnd(Coordinate from) const;
		bool findText(Coordinate from, const std::string& text, bool caseSensitive, bool wholeWord, Coordinate& start, Coordinate& end) const;

		// see if document was updated this frame (can only be called once)
		bool isUpdated() { auto result = updated; updated = false; return result; }
		void resetUpdated() { updated = false; }

		// utility functions
		bool isWholeWord(Coordinate start, Coordinate end) const;
		bool isEndOfLine(Coordinate from) const { return getIndex(from) == at(from.line).size(); }
		bool isLastLine(int line) const { return line == lineCount() - 1; }
		Coordinate normalizeCoordinate(Coordinate coordinate) const;

	private:
		int tabSize = 4;
		int maxColumn = 0;
		bool updated = false;
	} document;

	// single action to be performed on text as part of a larger transaction
	class Action {
	public:
		// action types
		enum class Type : char {
			insertText,
			deleteText
		};

		// constructors
		Action() = default;
		Action(Type t, Coordinate s, Coordinate e, const std::string& txt) : type(t), start(s), end(e), text(txt) {}

		// properties
		Type type;
		Coordinate start;
		Coordinate end;
		std::string text;
	};

	// a collection of actions for a complete transaction
 	class Transaction : public std::vector<Action> {
	public:
		// access state before/after transactions
		void setBeforeState(const Cursors& cursors) { before = cursors; }
		const Cursors& getBeforeState() const { return before; }
		void setAfterState(const Cursors& cursors) { after = cursors; }
		const Cursors& getAfterState() const { return after; }

		// add actions by type
		void addInsert(Coordinate start, Coordinate end, std::string text) { emplace_back(Action::Type::insertText, start, end, text); };
		void addDelete(Coordinate start, Coordinate end, std::string text) { emplace_back(Action::Type::deleteText, start, end, text); };

		// get number of actions
		int actions() const { return static_cast<int>(size()); }

	private:
		// properties
		Cursors before;
		Cursors after;
	};

	// transaction list to support do/undo/redo
 	class Transactions : public std::vector<std::shared_ptr<Transaction>> {
	public:
		// reset the transactions
		void reset();

		// create a new transaction
		static std::shared_ptr<Transaction> create() { return std::make_shared<Transaction>(); }

		// add a transaction to the list, execute it and make it undoable
		void add(std::shared_ptr<Transaction> transaction);

		// undo the last transaction
		void undo(Document& document, Cursors& cursors);

		// redo the last undone transaction;
		void redo(Document& document, Cursors& cursors);

		// get status information
		size_t getUndoIndex() const { return undoIndex; }
		bool canUndo() const { return undoIndex > 0; }
		bool canRedo() const { return undoIndex < size(); }

	private:
		size_t undoIndex = 0;
	} transactions;

	// text colorizer (handles language tokenizing)
	class Colorizer {
	public:
		// update colors in entire document
		void updateEntireDocument(Document& document, const Language* language);

		// update colors in changed lines in specified document
		void updateChangedLines(Document& document, const Language* language);

	private:
		// update color in a single line
		State update(Line& line, const Language* language);

		// see if string matches part of line
		bool matches(Line::iterator start, Line::iterator end, const std::string& text);

		// set color for specified range of glyphs
		void setColor(Line::iterator start, Line::iterator end, Color color) { while (start < end) (start++)->color = color; }
	} colorizer;

	// details about bracketed text
	class BracketPair {
	public:
		BracketPair(ImWchar sc, Coordinate s, ImWchar ec, Coordinate e, int l) : startChar(sc), start(s), endChar(ec), end(e), level(l) {}
		ImWchar startChar;
		Coordinate start;
		ImWchar endChar;
		Coordinate end;
		int level;

		bool isAfter(Coordinate location) const { return start > location; }
		bool isAround(Coordinate location) const { return start < location && end >= location; }
	};

	class Bracketeer : public std::vector<BracketPair> {
	public:
		// reset the bracketeer
		void reset();

		// update the list of bracket pairs in the document and colorize the relevant glyphs
		void update(Document& document);

		// find relevant brackets
		iterator getEnclosingBrackets(Coordinate location);
		iterator getEnclosingCurlyBrackets(Coordinate first, Coordinate last);
		iterator getInnerCurlyBrackets(Coordinate first, Coordinate last);

		// utility functions
		static bool isBracketCandidate(Glyph& glyph) {
			return glyph.color == Color::punctuation ||
				glyph.color == Color::matchingBracketLevel1 ||
				glyph.color == Color::matchingBracketLevel2 ||
				glyph.color == Color::matchingBracketLevel3 ||
				glyph.color == Color::matchingBracketError;
		}
	} bracketeer;

	// access the editor's text
	void setText(const std::string& text);
	void clearText();

	// render (parts of) the text editor
	void render(const char* title, const ImVec2& size, bool border);
	void renderSelections();
	void renderMarkers();
	void renderMatchingBrackets();
	void renderText();
	void renderCursors();
	void renderMargin();
	void renderLineNumbers();
	void renderDecorations();
	void renderScrollbarMiniMap();
	void renderPanScrollIndicator();
	void renderFindReplace(ImVec2 pos, float width);

	// keyboard and mouse interactions
	void handleKeyboardInputs();
	void handleMouseInteractions();

	// manipulate selections/cursors
	void selectAll();
	void selectLine(int line);
	void selectLines(int startLine, int endLine);
	void selectRegion(int startLine, int startColumn, int endLine, int endColumn);
	void selectToBrackets(bool includeBrackets);
	void growSelectionsToCurlyBrackets();
	void shrinkSelectionsToCurlyBrackets();

	void cut();
	void copy() const;
	void paste();
	void undo();
	void redo();

	// access cursor locations
	void getCursor(int& line, int& column, size_t cursor) const;
	void getCursor(int& startLine, int& startColumn, int& endLine, int& endColumn, size_t cursor) const;
	std::string	getCursorText(size_t cursor) const;

	// scrolling support
	void makeCursorVisible();
	void scrollToLine(int line, Scroll alignment);

	// find/replace support
	void selectFirstOccurrenceOf(const std::string& text, bool caseSensitive, bool wholeWord);
	void selectNextOccurrenceOf(const std::string& text, bool caseSensitive, bool wholeWord);
	void selectAllOccurrencesOf(const std::string& text, bool caseSensitive, bool wholeWord);
	void addNextOccurrence();
	void selectAllOccurrences();

	void replaceTextInCurrentCursor(const std::string& text);
	void replaceTextInAllCursors(const std::string& text);

	void openFindReplace();
	void find();
	void findNext();
	void findAll();
	void replace();
	void replaceAll();

	// marker support
	void addMarker(int line, ImU32 lineNumberColor, ImU32 textColor, const std::string& lineNumberTooltip, const std::string& textTooltip);
	void clearMarkers();

	// cursor/selection functions
	void moveUp(int lines, bool select);
	void moveDown(int lines, bool select);
	void moveLeft(bool select, bool wordMode);
	void moveRight(bool select, bool wordMode);
	void moveToTop(bool select);
	void moveToBottom(bool select);
	void moveToStartOfLine(bool select);
	void moveToEndOfLine(bool select);
	void moveTo(Coordinate coordinate, bool select);

	// add/delete characters
	void handleCharacter(ImWchar character);
	void handleBackspace(bool wordMode);
	void handleDelete(bool wordMode);

	// add/delete lines
	void removeSelectedLines();
	void insertLineAbove();
	void insertLineBelow();

	// transform selected lines
	void indentLines();
	void deindentLines();
	void moveUpLines();
	void moveDownLines();
	void toggleComments();

	// transform selections (filter function should accept and return UTF-8 encoded strings)
	void filterSelections(std::function<std::string(std::string)> filter);
	void selectionToLowerCase();
	void selectionToUpperCase();

	// transform entire document (filter function should accept and return UTF-8 encoded strings)
	void stripTrailingWhitespaces();
	void filterLines(std::function<std::string(std::string)> filter);
	void tabsToSpaces();
	void spacesToTabs();

	// transaction functions
	// note that strings must be UTF-8 encoded
	std::shared_ptr<Transaction> startTransaction();
	bool endTransaction(std::shared_ptr<Transaction> transaction);

	void insertTextIntoAllCursors(std::shared_ptr<Transaction> transaction, const std::string& text);
	void deleteTextFromAllCursors(std::shared_ptr<Transaction> transaction);
	void autoIndentAllCursors(std::shared_ptr<Transaction> transaction);
	Coordinate insertText(std::shared_ptr<Transaction> transaction, Coordinate start, const std::string& text);
	void deleteText(std::shared_ptr<Transaction> transaction, Coordinate start, Coordinate end);

	// editor options
	float lineSpacing = 1.0f;
	bool readOnly = false;
	bool autoIndent = true;
	bool showWhitespaces = true;
	bool showLineNumbers = true;
	bool showScrollbarMiniMap = true;
	bool showMatchingBrackets = true;
	bool completePairedGlyphs = true;
	bool overwrite = false;

	// rendering context
	ImFont* font;
	float fontSize;
	ImVec2 glyphSize;
	float lineNumberLeftOffset;
	float lineNumberRightOffset;
	float decorationOffset;
	float textOffset;
	float visibleHeight;
	int visibleLines;
	int firstVisibleLine;
	int lastVisibleLine;
	float visibleWidth;
	int visibleColumns;
	int firstVisibleColumn;
	int lastVisibleColumn;
	float verticalScrollBarSize;
	float horizontalScrollBarSize;
	float cursorAnimationTimer = 0.0f;
	bool ensureCursorIsVisible = false;
	int scrollToLineNumber = -1;
	Scroll scrollToAlignment = Scroll::alignMiddle;
	bool showMatchingBracketsChanged = false;
	bool languageChanged = false;

	float decoratorWidth = 0.0f;
	std::function<void(Decorator&)> decoratorCallback;

	std::function<void(int line)> lineNumberContextMenuCallback;
	std::function<void(int line, int column)> textContextMenuCallback;
	int contextMenuLine = 0;
	int contextMenuColumn = 0;

	static constexpr int leftMargin = 1; // margins are expressed in glyphs
	static constexpr int decorationMargin = 1;
	static constexpr int textMargin = 2;
	static constexpr int cursorWidth = 1;

	// find and replace support
	std::string findButtonLabel = "Find";
	std::string findAllButtonLabel = "Find All";
	std::string replaceButtonLabel = "Replace";
	std::string replaceAllButtonLabel = "Replace All";
	bool findReplaceVisible = false;
	bool focusOnEditor = true;
	bool focusOnFind = false;
	std::string findText;
	std::string replaceText;
	bool caseSensitiveFind = false;
	bool wholeWordFind = false;

	// interaction context
	float lastClickTime = -1.0f;
	ImWchar completePairCloser = 0;
	Coordinate completePairLocation;
	bool panMode = true;
	bool panning = false;
	bool scrolling = false;
	ImVec2 scrollStart;
	bool showPanScrollIndicator = true;

	// color palette support
	void updatePalette();
	static Palette defaultPalette;
	Palette paletteBase;
	Palette palette;
	float paletteAlpha;

	// language support
	const Language* language = nullptr;
};
