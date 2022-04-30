<div id="top"></div>

<!-- ABOUT THE PROJECT -->

## About Final Project

The object of the final project is to build a high speed memory controller using Verilog. The memory controller is one of the most complex components in a cpu die. It is responsible for, among others, recieving read and write requests from the cpu, creating a read and write queue, refreshing DRAM cells, managing row buffer management policies, and controlling address mapping schemes.

<br />
<div align="center">
  <a href="https://github.com/Nesathurai/advanced_computer_systems.git">
    <img src="images/memoryController.png" alt="250MB" width="900">
  </a>
<div align="left">

<div align="center">
Source: Professor Tong Zhang's Part3.pdf in Advanced Computer Systems, ECSE 4961 Spring 2022. 
<div align="left">

The final project implements the following features:

- feature A
- feature B

<p align="right">(<a href="#top">back to top</a>)</p>

### Installation and Usage

1. Install Vivado 2016.2

You should be able to create a free account with your rpi email address.
Download the windows copy [here.](https://www.xilinx.com/member/forms/download/xef.html?filename=Xilinx_Vivado_SDK_2016.2_0605_1_Win64.exe)

Note: All testing was performed using Vivado 2016.2, but other similar software may work too.

```sh
open the installer and install Vivado 2016.2
enter email and password when prompted
```

[https://github.com/Nesathurai/advanced_computer_systems.git](https://github.com/Nesathurai/advanced_computer_systems.git)

2. Download github repo

```sh
git clone https://github.com/Nesathurai/advanced_computer_systems.git
```

3. Go to cloned folder and view verilog files

```sh
cd advanced_computer_systems
cd project5
cd archive
```

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- STRUCTURE -->

## Structure

The memory controller is split into multiple verilog files. Each file describes either a base level component, or a higher level component which combines other base level components. For example, the sorting function has the following structure: there is a sort_two file which sorts two inputs, then two sort_two blocks are combined to create sort_four, and then two sort_four are combined to create sort_eight etc.

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- Experimental Results -->

## Experimental Results

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- Conclusion -->

## Conclusion

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- CONTACT -->

## Contact

Allan Nesathurai - ahnesathurai@gmail.com

Bennett Young - bennett.young@comcast.net

Project Link: [https://github.com/Nesathurai/advanced_computer_systems](https://github.com/Nesathurai/advanced_computer_systems)

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- ACKNOWLEDGMENTS -->

## Acknowledgments

- [README Template](https://github.com/othneildrew/Best-README-Template)
- [ZSTD v1.5.2 Manual](https://raw.githack.com/facebook/zstd/release/doc/zstd_manual.html)

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
