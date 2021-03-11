# CS427-Project1

Created by Ian Kieswether
10 March 2021

Usage: ./wsu_crypt [-e encrypt | -d decrypt] [-k keyFile] [-i inputFile] [-o outputFile]

Files included:
  wsu_crypt.c - This contains all the code for encryption and decryption.
  key.txt - A text file containing a 64 bit key represented as a 16 character hexadecimal string.
  plaintext.txt - An example plaintext file to test encryption
  Makefile - self explanatory

To Run:

    In the terminal, run:
    $ make
    $ ./wsu_crypt -e -k key.txt -i plaintext.txt -o ciphertext.txt (encryption)
    or
    $ ./wsu_crypt -d -k key.txt -i ciphertext.txt -o decrypted.txt (decryption)

  Note: The command line options [-edkio] can be entered in any order but only -e or -d can be specified in a single command and not both
