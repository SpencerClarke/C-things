# C things

<p>Almost everything in here will be written in ANSI C for fun and practice and also to potentially use some of the written programs for adding minor convenience to tasks.</p>

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
</ol>
</p>


