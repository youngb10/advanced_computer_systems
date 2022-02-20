<div id="top"></div>

<!-- ABOUT THE PROJECT -->
## About The Project

The objective of this project is to implement a C/C++ module that uses multiple threads to compress an input data stream. The input file is split into several files, and each file is dispatched to another thread to be broken down further and compressed. The compressed files are then recombined into a compressed file in the same order as the input. 

<p align="right">(<a href="#top">back to top</a>)</p>



### Installation and Usage

[https://www.howtoinstall.me/ubuntu/18-04/zstd/](https://www.howtoinstall.me/ubuntu/18-04/zstd/)
1. Install zstd by entering the following commands in the terminal:
  ```sh 
  sudo apt update
  sudo apt install zstd
  ```
[https://github.com/Nesathurai/advanced_computer_systems.git](https://github.com/Nesathurai/advanced_computer_systems.git)
2. Download github repo 
  ```sh
  git clone https://github.com/Nesathurai/advanced_computer_systems.git
  ```
3. Go to cloned folder
  ```sh 
  cd advanced_computer_systems
  cd project2
  cd files
  ```
4. To compress a small file: 
  ```sh
  make pthreads_streaming_compression
  ./pthreads_streaming_compression inputs_small.txt
  ```
5. To decompress a small file:
  
  This is not multithreaded, only do this to verify correct compression. 
  ```sh
  make decompression
  ./decompression test_files/small_github_users_sample_set.tar.zst
  ```
6. Compress custom file:
  
  While in files folder, create a new text file called:
  inputs_custom.txt
  Open inputs_small.txt, and copy the contents to inputs_custom.txt
  Then, change the last line to the desired file name
  Change the buffer size to desired value (around 10MB is good, too small and the program will not run properly)
  Change the desired number of threads (best performance will be the maximum number of physical threads on your system)
  Change the compression level to the desired level
  Then:
  ```sh
  ./pthreads_streaming_compression inputs_custom.txt
  ```

<p align="right">(<a href="#top">back to top</a>)</p>

5. Enter how many simultaneous threads you would like once running the program. 



<!-- STRUCTURE -->
## Structure

The overall structure of the program entails the main thread splitting the file to be compressed into several smaller files, dispatching one thread to each file to compress, and reassembling the compressed files in the correct order to obtain a compressed version of the original file. 

The program starts by reading the data from the inputs file, such as the number of worker threads and the size of the compression buffer, then using that data to split the file that will be compressed into several smaller files. The number of smaller files that are created is the same as the total number of threads. Each thread will compress one of these smaller files. 

Before compression, the main thread decides how many bytes each of the worker threads will handle. A struct is used to deliver the necessary data to the threads when they are created, and each of the threads is given an equal part of the original file. Each thread then works through the compression in parallel. 

Each smaller file that is being compressed is split into blocks that are the size of the compression buffer. The threads compress each block and continuously update their output file. Each thread has its own output file which will be the compressed version of each thread's input file. The main thread then reassembles these output files from each worker thread to a compressed version of the original file.

The compressed file is output, and all used memory is released. The program ends by outputting statistics of the compression including the time taken, the total size of the compressed file, and the compression ratio (which is the input size over the output size). 

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- Experimental Results -->
## Experimental Results

We were unable to obtain experimental results due to the seg faults that were occurring. 

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- Conclusion -->
## Conclusion

As hardware improvements gradually slow, one of the most significant ways to improve performance is through software-level parallelism. This is precisely what multithreading accomplishes. Multithreading is particularly useful and efficient in cases such as these, where there is a large amount of data that has little to no dependencies. Instead of a single thread moving through the independent calculations and compression, multiple threads are able to perform the same calculations in parallel without losses. 

Despite the fact that multithreading is a great method of improving performance, there are holdups. First, and perhaps most importantly, only a certain number of additional threads will be useful. Most computers cannot run more than 16 threads simultaneously, with 8 cores and 2 threads running in each. This is also supported by our experimental results, where the marginal benefit of each additional thread was inconsequential after 8 (4 cores with 2 threads each). Also, there is additional overhead required for dispatching these additional threads. When the data has dependencies, there are also additional steps that must be taken to preserve the accuracy of the output. However, these downsides are nothing compared to the benefits in many scenarios, and multithreading is a crucial tool in today's computing environment. 

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- CONTACT -->
## Contact

Allan Nesathurai - ahnesathurai@gmail.com
Bennett Young - bennett.young@comcast.net

Project Link: [https://github.com/Nesathurai/advanced_computer_systems](https://github.com/Nesathurai/advanced_computer_systems)

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

* [README Template](https://github.com/othneildrew/Best-README-Template)
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
