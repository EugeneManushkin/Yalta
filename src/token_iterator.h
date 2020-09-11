#pragma once

#include "token.h"

#include <cstdio>
#include <string>

class CharIterator
{
public:
  CharIterator(FILE* f);
  CharIterator& operator++();
  char operator*() const;
  long GetLineNumber() const;
  long GetLineOffset() const;

private:
  FILE* File;
  char Current;
  long LineNumber;
  long LineOffset;
};

class TokenIterator
{
public:
  TokenIterator(FILE* f);
  TokenIterator& operator++();
  Token const& operator*() const;

private:
  CharIterator Iter;
  Token Current;
};
