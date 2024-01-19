#include <cstdio>
#include <iostream>
#include <openssl/evp.h>
#include <random>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <vector>

#include "random.hpp"
#include "genrex.hpp"

std::vector<uint64_t> sha256(const std::string& input);
uint64_t get_seed(std::string &seed);
uint64_t pseudo_rdm(uint64_t seed, uint64_t cursor);
std::string read_password();
void disableEcho();
void enableEcho();


int main(int argc, char* arge[]) {
    if (argc != 3) {
        fprintf(stderr, "%s <username> <regex>\n", arge[0]);
        exit(10);
    }
    
    std::string password = read_password();
    std::string username(arge[1]);
    std::string str_seed = username + password;
    char *regex = arge[2];

    uint64_t seed = get_seed(str_seed);
    Random rdm(seed, pseudo_rdm);
    genrex::GenRex generator(regex, rdm);

    std::string generate = generator.generate_string();
    printf("Generated password is : %s\n", generate.c_str());
    
    return 0;
}

std::vector<uint64_t> sha256(const std::string& input)
{
   unsigned char hash[EVP_MAX_MD_SIZE];
   unsigned int len;
   EVP_MD_CTX *mdctx;
   const EVP_MD *md;

   md = EVP_sha256();
   if(!(mdctx = EVP_MD_CTX_new())) fprintf(stderr, "ERROR\n");

   if(1 != EVP_DigestInit_ex(mdctx, md, NULL)) fprintf(stderr, "ERROR\n");
   if(1 != EVP_DigestUpdate(mdctx, input.c_str(), input.length())) ;
   if(1 != EVP_DigestFinal_ex(mdctx, hash, &len)) fprintf(stderr, "ERROR\n");

   EVP_MD_CTX_free(mdctx);

   std::vector<uint64_t> result;
   for (size_t i = 0; i < len / sizeof(uint64_t); ++i) {
       uint64_t* word = reinterpret_cast<uint64_t*>(hash + i * sizeof(uint64_t));
       result.push_back(*word);
   }
   
   return result;
}

uint64_t get_seed(std::string &seed)
{
    std::vector<uint64_t> hash = sha256(seed);
    uint64_t total = 0;
    for (const auto& value : hash)
        total ^= value;
    return total;
}

uint64_t pseudo_rdm(uint64_t seed, uint64_t cursor)
{
    srand(((seed >> 32) ^ (seed + cursor)) & UINT32_MAX);
    return rand() | (uint64_t)(rand()) << 32;
}

void disableEcho()
{
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void enableEcho()
{
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

std::string read_password()
{
    printf("Enter the phrase pass\n> ");
    std::string password;
    char ch;
    disableEcho();
    while (std::cin.get(ch) && ch != '\n')
        password.push_back(ch);
    printf("\n");
    enableEcho();
    return password;
}