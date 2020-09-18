#include "parser.h"
#include "tags_writer.h"
#include "token_iterator.h"

#include <iostream>
#include <memory>
#include <stdexcept>

namespace
{
  std::string Trim(std::string&& str)
  {
    auto pos = str.find_last_not_of(" \t\r\n");
    str.resize(pos == std::string::npos ? str.length() : pos + 1);
    return std::move(str);
  }

  void ParseFile(char const* fname)
  try
  {
    std::shared_ptr<FILE> file(fopen(fname, "rb"), [](FILE* f){if (f) fclose(f);});
    if (!file)
      throw std::runtime_error("Failed to open file");

    TokenIterator iter(&*file);
    TagsWriter writer(fname, &*file, stdout);
    Parse(iter, writer);
  }
  catch(std::exception const& e)
  {
    std::cerr << "Error: " << e.what() << ". File: " << fname << std::endl;;
  }

  void Interactive()
  {
    std::string buf;
    while (std::getline(std::cin, buf))
      ParseFile(Trim(std::move(buf)).c_str());
  }
}

int main(int argc, char *argv[])
{
  try
  {
    if (argc <= 1)
      Interactive();

    for (--argc, ++argv; argc > 0; --argc, ++argv)
      ParseFile(argv[0]);
  }
  catch(std::exception const& e)
  {
    std::cerr << "Fatal error: " << e.what() << std::endl;
    return 1;
  }
}
