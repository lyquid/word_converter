# word_converter

## Building

The app uses CMake as building system. If you use an IDE, most probably the only you have to do is build and launch target. The command for building should be like:

    cmake --build --config Release --target all --
There's also a pre-compiled binary for Windows 64 in this [release](https://github.com/lyquid/word_converter/releases/download/1.0.0/word_converter.7z).

## Design decisions

The app uses the simple principle of divide and conquer. Everything is based around subdividing the problems into smaller ones.
The main problems to overcome are:

- Parsing and splitting the input file.
- Identify the numbers on each sentence.
- Calculating the numbers in digit format.
- Reconstruct the sentence.

The parsing part consists in the usage of standard STL functions for opening and reading the file. As the spec says, each sentence ends with a period character, so it's easy to split into sentences.  Later, each sentence is again splitted into words, or more "precisely" into strings that can be words, symbols, apostrophe, ... This makes it easier to later restore the sentence.

Then comes the identification part. This could be totally merged with the calculating phase; you can identify and calculate in one pass. I'd chosen no to because it's simpler and more maintainable and the individual identification of each word, assigning it a type and value, let's the door open for the app to become more complex. How you know whats a number and whatnot? Just comparing to the maps of numbers and exponents and checking if it's not a "adder" word (like "and").

The calculating part could seem pretty straightforward, until you input something like `One Hundred Eleven Million One Hundred Eleven Thousand One Hundred Eleven`. Then you realize you have a problem with the hundreds. It's a matter of playing with two accumulators, one for the current part of the number being calculated, and the other a global one. Keep in mind that the hundreds can be accumulated (they can appear multiple times in the same number).

The reconstruction is the easy part. Just push all the words into a single string, converting numbers to strings as needed.

I've used `std::strings` and `std::vectors` for convenience. Vectors are fast and there are no insert and delete operations in the middle or beginning of the vectors.
