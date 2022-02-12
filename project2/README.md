<div id="top"></div>

<!-- ABOUT THE PROJECT -->
## About The Project

The objective of this project is to implement a C/C++ module that uses multiple threads to compress an input data stream. The input file is split into 16kB chunks, and each 16kB chunk is dispatched to another thread to be compressed. The compressed blocks are then read back in order and written to an output file. 

<p align="right">(<a href="#top">back to top</a>)</p>



### Installation and Usage

1. Clone the GitHub repo for Project 2 and open the folder.

2. Verify that ZSTD is installed. Verify you are on a Linux OS. 

3. Download silesia.txt (~240MB uncompressed) from the ECSE-4961 Webex Teams, place it in the Project 2 directory.

4. Run the following commands inside the Project 2 directory to compile:
  ```sh
  gcc -pthread -O3 main.c -lzstd
  ./a.out FILE
  ```

5. Enter how many simultaneous threads you would like once running the program. 

**Unfortunately, we were unable to get the ZSTD compression to run without seg faults. The program's intended execution is detailed in the 'Structure' section.**

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- STRUCTURE -->
## Structure

The overall structure of the program entails one thread reading data from the input file, allocating space in memory for the compression, receiving compressed blocks, and writing the compressed blocks to the output, while a number of other threads implements the ZSTD compression. The number of threads is user configurable, with a minimum of two. The 'silesia.txt' file is used for compression. 

The program starts by reading data in from the file to be compressed. The number of total blocks that need to be compressed is determined using the size of this file. The program takes input from the user on how many threads should be created. Using these values, other critical information is calculated such as the number of blocks each thread will handle, the size of the last block, and the thread which will handle the last block. 

The threads that will be used for compression are then created. Because a thread can typically only read in one argument, a new struct must be created to hold all of the required information for each thread. This information includes the thread id, the memory locations of the input and output, and other values previously calculated. All of the threads that do the compression are forked at the same time, while the original thread is used for controls and gathering the output blocks. 

In the compression function which each compression thread executes, the data is first retrieved from the struct. The data from the struct is used to determine which threads do which blocks from the input, and the ZSTD_compress functions are run. Once the compression is finished, a global array is updated which allows the main thread to see the progress. Each thread is responsible for independent elements, so no threads write over each other. 

Back in the original thread, a while loop is constructed so that the thread will only continue and write to the output when the next compressed block is ready. This process repeats for as many blocks as there are, receiving the length of each compressed block from the global array. After the main thread writes all of the blocks to the output file, both the input and output files are closed, and the program ends. 

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- Experimental Results -->
## Experimental Results

We were unable to obtain experimental results due to the seg faults that were occurring. 

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- Conclusion -->
## Conclusion

As hardware improvements gradually slow, one of the most significant ways to improve performance is through software-level parallelism. This is precisely what multithreading accomplishes. Multithreading is particularly useful and efficient in cases such as these, where there is a large amount of data that has no dependencies. Instead of a single thread moving through the independent calculations and compression, multiple threads are able to perform the same calculations in parallel without losses. There is some amount of overhead that is involved with creating and managing threads, especially since the operating system has to get involved, but that overhead is absolutely worth it when there are large amounts ofcalculations to be done that are independent of each other. 

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- CONTACT -->
## Contact

Allan Nesathurai - ahnesathurai@gmail.com - 289 689 4248
Bennett Young - bennett.young@comcast.net

Project Link: [https://github.com/Nesathurai/advanced_computer_systems](https://github.com/Nesathurai/advanced_computer_systems)

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

* [README Template](https://github.com/othneildrew/Best-README-Template)
* [Intel Intrinsic's Guide](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html)
* [ZSTD v1.5.2 Manual](https://raw.githack.com/facebook/zstd/release/doc/zstd_manual.html)
* [POSIX Thread Programming](https://hpc-tutorials.llnl.gov/posix/)

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/github_username/repo_name.svg?style=for-the-badge
[contributors-url]: https://github.com/youngb10
[forks-shield]: https://img.shields.io/github/forks/github_username/repo_name.svg?style=for-the-badge
[forks-url]: https://github.com/github_username/repo_name/network/members
[stars-shield]: https://img.shields.io/github/stars/github_username/repo_name.svg?style=for-the-badge
[stars-url]: https://github.com/github_username/repo_name/stargazers
[issues-shield]: https://img.shields.io/github/issues/github_username/repo_name.svg?style=for-the-badge
[issues-url]: https://github.com/github_username/repo_name/issues
[license-shield]: https://img.shields.io/github/license/github_username/repo_name.svg?style=for-the-badge
[license-url]: https://github.com/github_username/repo_name/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/linkedin_username
[product-screenshot]: images/screenshot.png
