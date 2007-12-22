#ifndef _UNCOPYABLE_H_
#define _UNCOPYABLE_H_

class Uncopyable
{
protected:
  Uncopyable(void) {};
  virtual ~Uncopyable(void) {};

public:
  Uncopyable(Uncopyable const&);
  Uncopyable& operator=(Uncopyable const&);
};

#endif // _UNCOPYABLE_H_
