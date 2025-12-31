template <typename T, unsigned long S>
struct swar
{
  T D[ S ];

  swar &operator+=( swar other );
  swar &operator-=( swar other );
  swar &operator*=( swar other );
  // Long division/modulo for swar.
  swar &operator/=( swar other );
  swar &operator%=( swar other );
  swar  operator++( int );
  swar  operator--( int );
  swar &operator<<=( int );
  swar &operator>>=( int );
  swar &operator&=( swar other );
  swar &operator|=( swar other );
  swar &operator^=( swar other );
  swar &operator++();
  swar &operator--();
};
