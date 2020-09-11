#pragma once

#include <string>

enum class SequenceType
{
  None = 0,
  String,
  Comment,
  Operator,
  Identifier,
  Number,
};

struct Token
{
  SequenceType Type;
  std::string Sequence;
  long LineNum;
  long LineOffset;
};
