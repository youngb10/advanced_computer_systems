<div id="top"></div>

<!-- ABOUT THE PROJECT -->
## About The Project

The objective of this project is to implement a dictionary encoder. This dictionary encoder scans the to-be-compressed data to build the dictionary of unique data, and replaces each data item with its location ID in the dictionary. A query is also implemented to count the number of occurrences of each piece of data. Dictionary encoding is widely used in real-world data analytics systems, and is therefore useful to understand and implement. 

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
  cd project4
  ```
4. Run dictionary encoder with small size columns: 
  ```sh
  g++ main.cpp -o main.out -pthread
  ./main.out inputs_small.txt
  ```
5. To run the program using other size columns:
  ```sh
  ./main.out inputs_medium/large.txt
  ```
6. To run the program using different parameters, modify the corresponding inputs file. For example, to change the optimization level to 0 when running the dictionary encoding for small size columns, edit that parameter in the inputs_small.txt file. 

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- STRUCTURE -->
## Structure

The main structure of the program is two dictionaries set up using hash tables for better performance and more consistent access times. One dictionary is setup using the key-value pairs, and the second is set up with value-key pairs. This enables both a key to be searched by value, and a value to be searched by key. 

The implementation is dependent upon what level of optimization is chosen by the user. There are three different levels of optimization that can be chosen by changing a parameter in the input file. The optimization levels are 0, 1, and 2. 

The optimization level 0 is the most basic implementation of the dictionary encoder. Optimization 0 iterates through the input file. If the encountered word is new, it assigns a key to the word, adds that key to the output file, and increments the key for the next new word. If the encountered word is already in the dictionary, the existing key for that word is still added to the output file. 

Optimization level 1 is similar to 0, except a frequency dictionary is implemented. The words that are encountered more frequently are given smaller keys, reducing the average access time significantly. 

Optimization level 2 is similar to 1, except multithreading is implemented to further speed up the dictionary creation. The input file is split up into as many sections as there are threads (with the number of threads being user-configurable). The threads are then dispatched to create the dictionaries. Once they are all finished, the threads are joined and the final dictionary is assembled. 

Decompression is also implemented to decode the output file. The decompression replaces the keys in the output files with the corresponding values in the dictionary. 

A query function is implemented as well. When the inputs are taken in and the query function is called, the program goes through the file and checks if an item is there. Whenever the item is spotted, it increments a counter. 

A more performance-efficient way to implement the query function would be to create counters for each dictionary entry upon dictionary creation, but this would significantly increase the memory space overhead. 

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- Experimental Results -->
## Experimental Results

The following data was obtained using a Virtual Linux Machine with 4 cores, each capable of running two threads, and 16GB of memory. The processor is a 2.3GHz Intel i9. 

Data collected includes results for small, medium, and large column sizes, using optimization levels of 0, 1, and 2 for each column size. Compression time is the time that is taken to carry out the dictionary encoding and generate the encoded column file. Decompression time is the time that it takes to decompress the output file using the dictionary. 

Small Column Size:

    Optimization 0 compression time:   0.163s
    Optimization 0 decompression time: 0.126s
    Optimization 1 compression time:   0.159s
    Optimization 1 decompression time: 0.127s
    Optimization 2 compression time:   0.179s
    Optimization 2 decompression time: 0.129s
    Query time:                        0.006s

Medium Column Size:

    Optimization 0 compression time:   1.475s
    Optimization 0 decompression time: 1.264s
    Optimization 1 compression time:   1.462s
    Optimization 1 decompression time: 1.247s
    Optimization 2 compression time:   1.488s
    Optimization 2 decompression time: 1.274s
    Query time:                        0.073s

Large Column Size:

    Optimization 0 compression time:   32.719s
    Optimization 0 decompression time: 27.623s
    Optimization 1 compression time:   31.324s
    Optimization 1 decompression time: 26.279s
    Optimization 2 compression time:   29.916s
    Optimization 2 decompression time: 26.428s
    Query time:                        1.480s

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- Analysis and Conclusion -->
## Analysis and Conclusion

The experimental results show that the optimizations do not significantly affect the performance for the small and medium size columns. They only begin to have a significant affect when the columns are larger. This is reasonable, as the overhead for frequency dictionary creation and multithreading will be higher compared to the computational workload when the columns are smaller. As the column sizes increase, the amount that is gained from using a frequency dictionary and multithreading increases. The query times are also relatively consistent with the dictionary creation times and decompression times. 

Dictionary coding is being widely used in real-world data analytics systems to compress data. Because of this, it is critical to understand how dictionary encoding operates and how to implement it. Dictionary encoding is particularly useful in cases in which the data has relatively low cardinality. Hash tables are also used to significantly speed up the look-up times. Other data structures such as B-trees may be used, but that will significantly increase the complexity and will provide smaller returns when the entire dictionary can be fit into memory in cases like this. 

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
