# C things

<p>Almost everything in here will be written in C for fun and practice and also to potentially use some of the written programs for adding minor convenience to tasks.</p>

---
## <a href="Verticalizer/">Verticalizer</a>
<p>This program takes in Japanese text and prints out the text in pages of specified width and height in vertical writing style. The first argument is an integer representing the width of each page. The second argument is an integer representing the height of each page. The third argument is a filename to read from. If none is provided, it will default to stdin. The fourth argument is a filename to write to. If none is provided, it will default to stdout.</p>

## <a href="Memorizer/">Memorizer</a>
<p>This program helps to learn Japanese words more effectively. It takes in a txt file containig lines of two possible configurations. Each line starts with a word's meaning, followed by a comma, followed by the word written in hiragana, and then optionally followed by another comma and the word written in kanji. It then tests the user on each term, prompting them to enter the word in kanji if a kanji writing was specified, or in hiragana otherwise. If a term is guessed correctly, it is removed from the list it tests the user on. If it is guessed incorrectly, it is moved to the back of the list and the user will be tested on it again at the end.</p>

## <a href="Sorting/">Sorting</a>
<p>This program takes in a number, produces that many random integers, and then sorts them using two variants of quicksort and compares the times it takes to sort them. The first one uses Unix forks and shared memory to have multiple processes working on sorting different partitions of the list, and stops generating forks after it is 8 layers into the recursion, thus creating 256 processes in total. This may have some speed advantage over the second one, which is just regular quick sort, for larger sets.</p>

## <a href="Datastructs/">Datastructs</a>
<p>This folder is going to have data structures that I write when I am bored. For now it contains the following things
<ol>
    <li>A binary search tree with implementations to insert values, check if values exist, and remove values <a href="Datastructs/BST"> here</a></li>
    <li>A linked list that sits in shared memory, where each node is a value and the shared memory ID of the next node. I came up with this when I was trying to think of a joke about not wanting to use malloc. <a href="Datastructs/SharedLinkedLists"> here</a></li>
    <li>A trie that can store a bunch of strings and pair them with ints. The integer value can then be retrieved for any given string, and the string with the highest integer value after some prefix can be obtained too. There is also functionality to remove nodes and free the entire tree. <a href="Datastructs/Trie">here</a></li>
    <li>A trie that uses wide chars and is specialized for converting words in hiragana to kanji. <a href="Datastructs/Trie">here</a></li>
</ol>
</p>

## <a href="Autocomplete/">Autocomplete</a>
<p>This folder contains a program that takes in a filename as it's first argument of a file containing words that should be sorted by frequency of usage, with the word at the top of the file being the most common. It will then repeatedly ask the user to enter in a string and it will draw from the txt file and try to perform an autocomplete function on the input using the trie in <a href="Datastructs/Trie">Datastructs/Trie</a>. If the typed string is already a word, it will do nothing. If it is not, it will predict the completion of the word. If "stop" is typed, the program will free all allocated memory and cease.
</p>

## <a href="KanjiConverter/">KanjiConverter</a>
<p>This folder contains multiple files that work together to create a converter to convert Japanese words written in hiragana into kanji. Firstly, there is a file called get_list.sh that gets a series of links from a file called links.txt that link to Kanshudo, a website for Japanese learners. Next, it curls those links into html files. Next, the files are converted into well-formed xhtml files using a java program called tagsoup. Next, the files are parsed using python's Minidom module to scrape a decently sized list of words written in Japanese from the xhtml files and outputs them to a file called temp.txt. Next, get_list.sh removes any duplicates from temp.txt and replaces it with out.txt The program "convert.c" can take it's filename in as a command line argument and build a prefix tree using the scraped data. It will then wait for the user to type in a word in hiragana and print out all of it's writings that were found. If the list of data is ordered from most common to least common, with the top being most common, which should roughly be the case here, the writings printed will be in order from most common to least common. If the user enters nothing, the program frees the prefix tree and exits.
</p>

## <a href="Threads/">Threads</a>
<p>This folder contains some experimental proof of concept programs that estimate pi and divide up the problem of estimation and distribute it among multiple threads. Two programs use monte carlo pi estimation using random points, and two of them calculate pi by estimating the definite integral of 4/(x²+1) over the interval 0 <= x <= 1. There are versions of both methods using pthreads, and OpenMP because I wanted to try using both facilities. In addition, there is also a program that compares regular quicksort with a multithreaded quicksort written with pthreads.
</p>


