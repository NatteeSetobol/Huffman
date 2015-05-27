#ifndef lettercount_h
#define lettercount_h
class lettercount
{
  public:
	unsigned char letter;
	unsigned int count;
	double prob;
	  lettercount();
	 ~lettercount();

};

lettercount::lettercount()
{
	count = 0;
	letter = 0;
	prob = 0.0;
}


lettercount::~lettercount()
{
}
#endif
