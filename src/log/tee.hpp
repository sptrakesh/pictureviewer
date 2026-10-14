//
// Created by Rakesh on 2019-05-15.
//
// http://wordaligned.org/articles/cpp-streambufs

#pragma once
#include <ostream>
#include <streambuf>

namespace spt::log
{
  class teestream;

  class teebuf: public std::streambuf
  {
  public:
    // Construct a streambuf which tees output to both input
    // streambufs.
    teebuf() {}
    teebuf(std::streambuf* sb1, std::streambuf* sb2) : sb1(sb1) , sb2(sb2) {}
  private:
    // This tee buffer has no buffer. So every character "overflows"
    // and can be put directly into the teed buffers.
    int overflow(int c) override
    {
      if (c == EOF)
      {
        return !EOF;
      }
      else
      {
        int const r1 = sb1->sputc(c);
        int const r2 = sb2->sputc(c);
        return r1 == EOF || r2 == EOF ? EOF : c;
      }
    }

    // Sync both teed buffers.
    int sync() override
    {
      int const r1 = sb1->pubsync();
      int const r2 = sb2->pubsync();
      return r1 == 0 && r2 == 0 ? 0 : -1;
    }
  private:
    std::streambuf* sb1;
    std::streambuf* sb2;
    friend class teestream;
  };

  class teestream : public std::ostream
  {
  public:
    // Construct an ostream which tees output to the supplied
    // ostreams.
    teestream(std::ostream& o1, std::ostream& o2) :
        std::ostream(&tbuf)
    {
      tbuf.sb1 = o1.rdbuf();
      tbuf.sb2 = o2.rdbuf();
    }

  private:
    teebuf tbuf;
  };
}
