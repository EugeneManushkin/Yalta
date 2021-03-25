#include "tags_writer.h"

#include <string>
#include <unordered_map>

namespace
{
  size_t constexpr EffectiveStringLen = 256;
  int constexpr CRETURN = '\r';
  int constexpr NEWLINE = '\n';
  int constexpr BACKSLASH = '\\';

  struct CtagsOptions
  {
    size_t patternLengthLimit;
    bool backward;
  };

  CtagsOptions Option = {EffectiveStringLen - 1, false};

/* COPIED AS IS FROM entry.c */
/*
 *  Tag entry management
 */

/*  This function copies the current line out to a specified file. It has no
 *  effect on the fileGetc () function.  During copying, any '\' characters
 *  are doubled and a leading '^' or trailing '$' is also quoted. End of line
 *  characters (line feed or carriage return) are dropped.
 */
static size_t appendInputLine (int putc_func (char , void *), const char *const line, void * data, bool *omitted)
{
	size_t length = 0;
	const char *p;
	int extraLength = 0;

	/*  Write everything up to, but not including, a line end character.
	 */
	*omitted = false;
	for (p = line  ;  *p != '\0'  ;  ++p)
	{
		const int next = *(p + 1);
		const int c = *p;

		if (c == CRETURN  ||  c == NEWLINE)
			break;

		if (Option.patternLengthLimit != 0 && length >= Option.patternLengthLimit &&
			/* Do not cut inside a multi-byte UTF-8 character, but safe-guard it not to
			 * allow more than one extra valid UTF-8 character in case it's not actually
			 * UTF-8.  To do that, limit to an extra 3 UTF-8 sub-bytes (0b10xxxxxx). */
			((((unsigned char) c) & 0xc0) != 0x80 || ++extraLength > 3))
		{
			*omitted = true;
			break;
		}
		/*  If character is '\', or a terminal '$', then quote it.
		 */
		if (c == BACKSLASH  ||  c == (Option.backward ? '?' : '/')  ||
			(c == '$'  &&  (next == NEWLINE  ||  next == CRETURN)))
		{
			putc_func (BACKSLASH, data);
			++length;
		}
		putc_func (c, data);
		++length;
	}

	return length;
}

  std::string GetLine(long lineOffset, FILE* f)
  {
    auto offset = ftell(f);
    fseek(f, lineOffset, SEEK_SET);
    char buffer[EffectiveStringLen + 1] = "";
    auto success = fgets(buffer, sizeof(buffer), f);
    fseek(f, offset, SEEK_SET);
    return success ? buffer : "";
  }

  int putc_func_callback(char c, void *file)
  {
    return fputc(c, static_cast<FILE*>(file));
  }

  void WriteRegex(long lineOffset, FILE* input, FILE* output)
  {
    fprintf(output, "/^");
    bool omitted = false;
    appendInputLine(&putc_func_callback, GetLine(lineOffset, input).c_str(), output, &omitted);
    fprintf(output, "%s/;\"", omitted ? "" : "$");
  }
}

TagsWriter::TagsWriter(char const* fname, FILE* fin, FILE* fout)
  : Filename(fname)
  , Input(fin)
  , Output(fout)
{
}

void TagsWriter::Write(Kind kind, std::deque<Token> const& scope)
{
  static std::unordered_map<Kind, std::string> KindChar = {
      {Kind::Local, "l"}
    , {Kind::Function, "f"}
    , {Kind::Table, "t"}
    , {Kind::Key, "k"}
    , {Kind::Assignment, "a"}
  };

  // scope not empty
  fprintf(Output, "%s\t%s\t", scope.back().Sequence.c_str(), Filename.c_str());
  WriteRegex(scope.back().LineOffset, Input, Output);
  fprintf(Output, "\t%s\tline:%ld", KindChar[kind].c_str(), scope.back().LineNum);
  for (auto i = scope.begin(); i != --scope.end(); ++i)
    fprintf(Output, "%s%s", i == scope.begin() ? "\tclass:" : ".", i->Sequence.c_str());

  fprintf(Output, "\n");
}
