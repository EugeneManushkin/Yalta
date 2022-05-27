#include "token_iterator.h"

#include <algorithm>
#include <deque>
#include <unordered_set>

namespace
{
  bool IsSpace(char c)
  {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
  }

  bool IsAlpha(char c)
  {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
  }

  bool IsNumeric(char c)
  {
    return c >= '0' && c <= '9';
  }

  bool IsSequenceComment(char first, char last, size_t length)
  {
    return length == 2 && first == '-' && last == '-';
  }

  bool IsSequenceShortString(char first, size_t length)
  {
    return length == 1 && (first == '\'' || first == '"' || first == '`');
  }

  bool IsSequenceLongString(char first, char middle, char last, size_t length)
  {
    return length >= 2 && first == '[' && last == '[' && (length < 3 || middle == '=');
  }

  bool IsSequenceString(char first, char middle, char last, size_t length)
  {
    return IsSequenceShortString(first, length) || IsSequenceLongString(first, middle, last, length);
  }

  bool IsSequenceStringPart(char first, char middle, char last, size_t length)
  {
    return length >= 2 && first == '[' && last == '=' && (length < 3 || middle == '=');
  }

  bool IsSequenceOperator(char first, char middle, char last, size_t length)
  {
    static const std::unordered_set<std::string> operators = {"//", "..", "==", ">=", "<=", "~=", "<<", ">>", "::"};
    char key[3] = {first, last};
    return (length == 2 && operators.find(key) != operators.end()) || (length == 3 && first == '.' && middle == '.' && last == '.');
  }

  bool IsSequenceIdentifier(char first, char last, size_t length)
  {
    return IsAlpha(first) && (length == 1 || IsAlpha(last) || IsNumeric(last));
  }

  bool IsSequenceNumber(char first, char last, size_t length)
  {
    return IsNumeric(first) && (length == 1 || IsAlpha(last) || IsNumeric(last));
  }

  char First(std::deque<char> const& sequence)
  {
    return sequence.empty() ? 0 : sequence.front();
  }

  char Middle(std::deque<char> const& sequence)
  {
    return sequence.size() > 2 ? sequence[sequence.size() - 2] : 0;
  }

  char Last(std::deque<char> const& sequence)
  {
    return sequence.empty() ? 0 : sequence.back();
  }

  SequenceType GetType(std::deque<char> const& sequence)
  {
    size_t length = sequence.size();
    auto first = First(sequence);
    auto middle = Middle(sequence);
    auto last = Last(sequence);
    return IsSequenceString(first, middle, last, length) ? SequenceType::String :
           IsSequenceComment(first, last, length) ? SequenceType::Comment :
           IsSequenceOperator(first, middle, last, length) ? SequenceType::Operator :
           IsSequenceIdentifier(first, last, length) ? SequenceType::Identifier :
           IsSequenceNumber(first, last, length) ? SequenceType::Number :
           SequenceType::None;
  }

  bool MayAdd(std::deque<char> const& sequence, char c)
  {
    size_t length = sequence.size() + 1;
    auto first = First(sequence);
    return sequence.empty()
        || IsSequenceComment(first, c, length)
        || IsSequenceLongString(first, Last(sequence), c, length)
        || IsSequenceStringPart(first, Last(sequence), c, length)
        || IsSequenceOperator(first, Last(sequence), c, length)
        || IsSequenceIdentifier(first, c, length)
        || IsSequenceNumber(first, c, length)
    ;
  }

  std::deque<char> AccumulateSequence(CharIterator& iter)
  {
    std::deque<char> sequence;
    for (; *iter != EOF && MayAdd(sequence, *iter); sequence.push_back(*iter), ++iter);
    return std::move(sequence);
  }

  void SkipSpace(CharIterator& iter)
  {
    for (; *iter != EOF && IsSpace(*iter); ++iter);
  }

  std::deque<char> GetClosingToken(std::deque<char> const& openingToken)
  {
    std::deque<char> result = openingToken;
    if (IsSequenceLongString(First(result), Middle(result), Last(result), result.size()))
    {
      result.front() = ']';
      result.back() = ']';
    }

    return std::move(result);
  }

  void SkipString(CharIterator& iter, std::deque<char> const& closingToken)
  {
    std::deque<char> buffer;
    bool escaped = false;
    for (; *iter != EOF && (escaped || buffer.size() != closingToken.size() || !std::equal(buffer.begin(), buffer.end(), closingToken.begin())); ++iter)
    {
      buffer.push_back(*iter);
      if (buffer.size() > closingToken.size())
      {
        escaped = closingToken.size() == 1 && !escaped && buffer.front() == '\\';
        buffer.pop_front();
      }
    }
  }

  void SkipComment(CharIterator& iter)
  {
    auto sequence = AccumulateSequence(iter);
    if (IsSequenceLongString(First(sequence), Middle(sequence), Last(sequence), sequence.size()))
      SkipString(iter, GetClosingToken(sequence));
    else
      for (; *iter != EOF && *iter != '\n'; ++iter);
  }

  Token NextToken(CharIterator& iter)
  {
    SkipSpace(iter);
    auto line = iter.GetLineNumber();
    auto lineOffset = iter.GetLineOffset();
    auto sequence = AccumulateSequence(iter);
    auto type = GetType(sequence);
    if (type == SequenceType::String)
      SkipString(iter, GetClosingToken(sequence));
    else if (type == SequenceType::Comment)
      SkipComment(iter);

    return {type, std::string(sequence.begin(), sequence.end()), line, lineOffset};
  }
}

CharIterator::CharIterator(FILE* f)
  : File(f)
  , Current(EOF)
  , LineNumber(1)
  , LineOffset(0)
{
  ++*this;
}

CharIterator& CharIterator::operator++()
{
  LineNumber += Current == '\n' ? 1 : 0;
  LineOffset = Current == '\n' ? ftell(File) : LineOffset;
  Current = fgetc(File);
  return *this;
}

int CharIterator::operator*() const
{
  return Current;
}

long CharIterator::GetLineNumber() const
{
  return LineNumber;
}

long CharIterator::GetLineOffset() const
{
  return LineOffset;
}

TokenIterator::TokenIterator(FILE* f)
  : Iter(f)
{
  ++*this;
}

TokenIterator& TokenIterator::operator++()
{
  for (Current = NextToken(Iter); !Current.Sequence.empty() && Current.Type == SequenceType::Comment; Current = NextToken(Iter));
  return *this;
}

Token const& TokenIterator::operator*() const
{
  return Current;
}
