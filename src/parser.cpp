#include "parser.h"
#include "tags_writer.h"
#include "token_iterator.h"

#include <unordered_set>

using TokenSequence = std::deque<Token>;

namespace
{
  bool Valid(TokenIterator const& iter)
  {
    return !(*iter).Sequence.empty();
  }

  bool IsIdentifier(Token const& token)
  {
    static std::unordered_set<std::string> keywords = {
      "and", "break", "do", "else", "elseif",
      "end", "false", "for", "function", "goto", "if",
      "in", "local", "nil", "not", "or", "repeat",
      "return", "then", "true", "until", "while"
    };

    return token.Type == SequenceType::Identifier && !keywords.count(token.Sequence);
  }

  bool IsFunction(Token const& token)
  {
    return !token.Sequence.compare("function");
  }

  bool SkipMatched(TokenIterator& iter, char const* sequence)
  {
    bool matched = false;
    if (matched = !(*iter).Sequence.compare(sequence))
      ++iter;

    return matched;
  }

  TokenSequence ParseDotSeparatedSequence(TokenIterator& iter)
  {
    std::deque<Token> sequence;
    while (Valid(iter) && IsIdentifier(*iter))
    {
      sequence.push_back(*iter);
      ++iter;
      if (!SkipMatched(iter, "."))
        break;
    }

    return std::move(sequence);
  }

  void ParseAssignment(TokenIterator& iter, TagsWriter& writer)
  {
    auto sequence = ParseDotSeparatedSequence(iter);
    if (!SkipMatched(iter, "="))
      return;

  //TODO: check repeated names threshold
    Kind kind = IsFunction(*iter) ? Kind::Function :
                sequence.size() > 1 ? Kind::Key :
                Kind::Assignment;
    writer.Write(kind, sequence);
  }

  std::deque<TokenSequence> ParseVariableList(TokenIterator& iter)
  {
    std::deque<TokenSequence> sequences;
    while (Valid(iter) && IsIdentifier(*iter))
    {
      sequences.push_back({*iter});
      ++iter;
      if (!SkipMatched(iter, ","))
        break;
    }

    return std::move(sequences);
  }

  void ParseLocal(TokenIterator& iter, TagsWriter& writer)
  {
    if (IsFunction(*iter))
      return;

    auto sequences = ParseVariableList(iter);
    if (SkipMatched(iter, "=") && IsFunction(*iter))
    {
      TokenSequence sequence = std::move(sequences.front());
      sequences.pop_front();
      writer.Write(Kind::Function, sequence);
    }

    for (auto const& sequence : sequences)
      writer.Write(Kind::Local, sequence);
  }

  void ParseFunction(TokenIterator& iter, TagsWriter& writer)
  {
    auto sequence = ParseDotSeparatedSequence(iter);
    if (!sequence.empty() && SkipMatched(iter, ":") && IsIdentifier(*iter))
    {
      sequence.push_back(*iter);
      ++iter;
    }

    if (!sequence.empty())
      writer.Write(Kind::Function, sequence);

    if (SkipMatched(iter, "("))
    {
      for (auto const& sequence : ParseVariableList(iter))
        writer.Write(Kind::Local, sequence);
    }
  }
}

void Parse(TokenIterator& iter, TagsWriter& writer)
{
  while (Valid(iter))
  {
    if (SkipMatched(iter, "local") || SkipMatched(iter, "for"))
    {
      ParseLocal(iter, writer);
    }
    else if (SkipMatched(iter, "function"))
    {
      ParseFunction(iter, writer);
    }
    else if (IsIdentifier(*iter))
    {
      ParseAssignment(iter, writer);
    }
    else
    {
      ++iter;
    }
  }
}
