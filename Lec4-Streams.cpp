/*
=====================================================================================
Lecture 4: Streams
=====================================================================================

1) What is a stream?
   A stream is C++'s general input/output facility: a sequence of characters flowing
   FROM a source (keyboard, file, string) or TO a destination (screen, file, string).
   Streams CONVERT between data of any type and its string (text) representation:
       std::cout << 5;   ---> the int 5 is converted to the text "5" and sent to the console
       std::cin >> x;    ---> the text "19" typed by the user is converted to the int 19

2) Streams are an abstraction
   An abstraction hides unnecessary details and provides a consistent interface.
   You read and write the SAME way (<< and >>) whether the endpoint is the console,
   a file or a string; only the type of stream changes.

3) Kinds of streams
   a) By direction:
      - input streams  (std::istream)  ---> only READ data with >>          e.g. std::cin
      - output streams (std::ostream)  ---> only WRITE data with <<         e.g. std::cout
      - input/output   (std::iostream) ---> both read and write             e.g. std::fstream,
                                                                                 std::stringstream
   b) By source/destination:
                       Read only            Write only            Read and write   Header
      console          std::cin             std::cout                              <iostream>
      file             std::ifstream        std::ofstream         std::fstream     <fstream>
      string           std::istringstream   std::ostringstream    std::stringstream <sstream>

4) The two stream operators: << and >>   (the arrow points in the direction the data flows)
   <<  stream INSERTION operator ("put to")
       stream << data      ---> data flows INTO an output stream
       std::cout << "Hi";        // print to the screen
       fout << "Hi";             // write to a file
   >>  stream EXTRACTION operator ("get from")
       stream >> variable  ---> data flows OUT OF an input stream into the variable,
                                converted to the variable's type
       std::cin >> name;         // read from the keyboard
       fin >> word;              // read from a file
   - Both can be chained, because each returns the stream itself:
       std::cout << "Age: " << age << '\n';
       std::cin >> name >> age;

5) Output streams (type std::ostream)
   - You can only SEND data to them, with <<. Data of any built-in type is converted to
     text and sent to the stream.
   - std::cout is the output stream connected to the console. It is a global object that
     <iostream> creates for us, ready to use:
       std::cout << 5 << std::endl;   // converts 5 to "5" and sends it to the console
   - Output FILE stream (std::ofstream): you must create your own object linked to a file:
       std::ofstream out("out.txt");  // out now writes to out.txt (created/overwritten)
       out << 5 << std::endl;         // out.txt contains 5
   - Output is BUFFERED: text is collected in memory and written in chunks. std::endl,
     std::flush or closing the stream forces the buffer to be written out.

6) Input streams (type std::istream)
   - You can only RECEIVE data from them, with >>. It reads text from the stream and
     converts it to the variable's type.
   - std::cin is the input stream connected to the console (keyboard), also a global object
     from <iostream>:
       int x;
       std::string str;
       std::cin >> x >> str;          // reads exactly one int, then one word
   - How std::cin works:
     * The first std::cin >> waits while the user types, until they press Enter.
     * The whole typed line goes into a BUFFER (memory).
     * Each >> skips leading whitespace (spaces, tabs, newlines), then reads only until
       the NEXT whitespace. Whitespace is "eaten": it never ends up in your variable.
     * Whatever is left in the buffer is used by the next >> (it won't wait for typing).
   - Think of an istream as a sequence of characters that >> consumes from the front.
   - Input FILE stream (std::ifstream): you must create your own object linked to a file:
       std::ifstream in("out.txt");
       std::string str;
       in >> str;                     // the first word of out.txt goes into str

7) When input goes wrong
       std::string str;  int x;  std::string otherStr;
       std::cin >> str >> x >> otherStr;    // user types: blah blah
   - str = "blah". Then >> x sees "blah", which is not an int ---> extraction FAILS:
     * x is set to 0 (no crash!)
     * the stream's FAIL bit is set, and from then on it ignores every further >>
       (otherStr stays empty)
   - Check the stream like a bool:  if (std::cin) { ... }  ---> true only if no error so far.
   - Reading until the end of a file, the right way:
       while (input >> value) { ... }   // stops at end of file OR at bad data
     Don't loop on while (!input.eof()): eof is only set AFTER a read fails, so the last
     value gets processed twice, and bad data causes an infinite loop.

8) std::getline: read a whole line
       std::istream& getline(std::istream& is, std::string& str, char delim = '\n');
   - Clears str, then extracts characters from is and stores them in str until:
     * the delimiter is found (default '\n'): it is extracted but NOT stored in str
     * the end of the stream is reached ---> EOF bit set (check with is.eof())
     * if no characters could be extracted at all ---> FAIL bit set (check with is.fail())
   - getline vs >> :
                          >>                                std::getline
     reads up to          next whitespace (one "word")      the delimiter (a whole line)
     can stop at          whitespace only                   any delimiter you choose
     produces             any built-in type (int, double)   only std::string
   - PITFALL: mixing them. After std::cin >> x, the '\n' from pressing Enter is still in the
     buffer, so the next std::getline reads an EMPTY line. Fix: skip it first with
     std::cin >> std::ws (skips whitespace) or std::cin.ignore().

9) String streams (<sstream>)
   - A stream whose endpoint is a std::string in memory instead of the console or a file.
   - Lets you use << and >> on a string, exactly like the other streams:
     * std::istringstream (read only):  built FROM a string; read it word by word / value by
       value with >>. Great for parsing:  std::istringstream iss("Emily CA 19");
     * std::ostringstream (write only): send any data into it with <<; get the final text
       with .str(). Great for building strings.
     * std::stringstream: both read and write.

10) What can be printed with << ?
   Built-in types (int, double, char, bool, ...), C strings and std::string work out of the
   box. Your own types (e.g. struct Student) and most containers (e.g. std::vector) do NOT:
   you must write operator<< for them yourself (operator overloading, a later lecture).

Summary
   - Streams convert between data of any type and its string representation.
   - Every stream has an endpoint: the console (cin/cout), a file (fstreams) or a string
     (stringstreams).
   - stream << data ---> send data (as text) to a stream.
   - stream >> data ---> extract text from a stream and convert it to data's type.
*/
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

struct Student {
    std::string name;
    std::string school;
    int age;
};

// =====================================================================================
// 1) Output with << (std::cout)
// =====================================================================================
int main1() {
    std::cout << "Hello World" << std::endl;  // std::endl = newline + flush the stream

    Student s{"Emily", "CA", 19};
    // std::cout << s << std::endl;  // ERROR: no operator<< for Student (we write it later)
    std::cout << s.name << std::endl;  // OK: s.name is a std::string, which << understands

    // Print each member yourself, chaining << :
    std::cout << s.name << " studies in " << s.school << " and is " << s.age << '\n';
    return 0;
}

// =====================================================================================
// 2) Input with >> (std::cin) and files (std::ofstream / std::ifstream)
// =====================================================================================
int main2() {
    // Read from the keyboard: >> reads ONE word (stops at a space or newline).
    std::string student_input;
    std::cout << "Type a word and press Enter: ";
    std::cin >> student_input;
    std::cout << "You typed: " << student_input << '\n';

    // Write to a file with << . Opening an ofstream creates (or overwrites) data.txt.
    std::ofstream fout("data.txt");
    fout << "I am writing to this file.";
    fout.close();  // close (and flush) the file so the text is really saved before we read it

    // Read from the file with >> : same operator as std::cin, just a different stream.
    std::ifstream fin("data.txt");
    std::string first_word;
    fin >> first_word;  // extracts "I" (stops at the first space)
    std::cout << "First word in the file: " << first_word << '\n';

    // To read the rest of the line (spaces included), use std::getline:
    std::string rest;
    std::getline(fin, rest);  // " am writing to this file."
    std::cout << "Rest of the line:" << rest << '\n';
    return 0;
}

// =====================================================================================
// 3) When input goes wrong: the fail bit, and the correct reading loop
// =====================================================================================
int main3() {
    // Simulate a user typing "blah blah" into: std::cin >> str >> x >> otherStr;
    // (an istringstream behaves exactly like std::cin, but needs no typing)
    std::istringstream fakeCin("blah blah");
    std::string str;
    int x = -1;
    std::string otherStr;
    fakeCin >> str >> x >> otherStr;  // ">> x" fails: "blah" is not an int

    std::cout << "str = \"" << str << "\", x = " << x << ", otherStr = \"" << otherStr << "\"\n";
    if (fakeCin) {
        std::cout << "success\n";
    } else {
        std::cout << "extraction failed: fail bit is set, x became 0, otherStr was skipped\n";
    }

    // Reading all numbers from a file. First create numbers.txt so the example works.
    std::ofstream out("numbers.txt");
    out << "10 20\n30\n";
    out.close();

    std::ifstream input("numbers.txt");
    int value = 0;
    while (input >> value) {  // correct: the loop stops when >> fails (end of file or bad data)
        std::cout << "read " << value << '\n';
    }
    // while (!input.eof()) { input >> value; ... }  // WRONG: see notes section 7
    return 0;
}

// =====================================================================================
// 4) std::getline vs >>, and the pitfall of mixing them
// =====================================================================================
int main4() {
    // Simulate a user typing their age, pressing Enter, then typing their full name.
    std::istringstream fakeCin("19\nEmily Smith\n");
    int age = 0;
    std::string fullName;

    fakeCin >> age;                  // reads 19, but leaves the '\n' in the buffer!
    std::getline(fakeCin, fullName);  // PITFALL: reads up to that '\n' ---> empty string
    std::cout << "age = " << age << ", name = \"" << fullName << "\"  <--- empty!\n";

    std::getline(fakeCin, fullName);  // the next getline gets the real line
    std::cout << "next getline: name = \"" << fullName << "\"\n";

    // Fix: after >>, skip the leftover whitespace before calling getline.
    std::istringstream fakeCin2("19\nEmily Smith\n");
    fakeCin2 >> age >> std::ws;  // std::ws eats the '\n' (and any other whitespace)
    std::getline(fakeCin2, fullName);
    std::cout << "with std::ws: age = " << age << ", name = \"" << fullName << "\"\n";

    // getline with a custom delimiter: split "Emily,CA,19" at the commas.
    std::istringstream csv("Emily,CA,19");
    std::string field;
    while (std::getline(csv, field, ',')) {
        std::cout << "field: " << field << '\n';
    }
    return 0;
}

// =====================================================================================
// 5) String streams: parse a string, or build a string
// =====================================================================================
int main5() {
    // istringstream: READ from a string with >> (parsing). Each >> converts to the right type.
    std::istringstream iss("Emily CA 19");
    Student s;
    iss >> s.name >> s.school >> s.age;  // "19" is converted to the int 19
    std::cout << "Parsed: " << s.name << ", " << s.school << ", " << s.age + 1
              << " next year\n";  // age is a real int now: we can do math with it

    // ostringstream: WRITE any data into it with <<, then get the text with .str().
    std::ostringstream oss;
    oss << s.name << " is " << s.age << " years old";  // int 19 is converted to "19"
    std::string sentence = oss.str();
    std::cout << "Built: " << sentence << " (" << sentence.size() << " characters)\n";
    return 0;
}

// =====================================================================================
// main: runs every example in order
// =====================================================================================
int main() {
    std::cout << "--- 1) Output with << ---\n";
    main1();
    std::cout << "--- 2) Input with >> and files ---\n";
    main2();
    std::cout << "--- 3) When input goes wrong ---\n";
    main3();
    std::cout << "--- 4) getline vs >> ---\n";
    main4();
    std::cout << "--- 5) String streams ---\n";
    main5();
    return 0;
}
