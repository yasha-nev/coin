#ifndef base58_hpp
#define base58_hpp

int EncodeBase58(char *source, int len, char result[], int reslen);
int DecodeBase58(char *str, int len, char *result);

#endif /* base58_hpp */
