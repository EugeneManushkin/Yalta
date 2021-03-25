#pragma once

#include "token.h"

#include <cstdio>
#include <deque>
#include <string>

enum class Kind
{
  Local,
  Function,
  Table,
  Key,
  Assignment,
};

class TagsWriter
{
public:
  TagsWriter(char const* fname, FILE* fin, FILE* fout);
  void Write(Kind kind, std::deque<Token> const& scope);

private:
  std::string Filename;
  FILE* Input;
  FILE* Output;
};
