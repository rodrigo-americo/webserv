/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 13:51:47 by ighannam          #+#    #+#             */
/*   Updated: 2026/07/03 18:08:31 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileSystem.hpp"
#include <iostream>

void    print_fs(FileSystem &fs);

int main()
{
    FileSystem fs("www");
    print_fs(fs);
    std::cout << "\nEntrando em cgi-bin \n";
    fs.cd("cgi-bin");
    print_fs(fs);
}

void    print_fs(FileSystem &fs)
{
    std::cout << "path antes: " << fs.path().getPath() << "\n";
    std::cout << "IsDir: " << fs.isDir() << "\n";
    std::cout << "IsFile: " << fs.isFile() << "\n";
    std::vector<Path> ls = fs.ls();
    if (ls.empty())
    {
        std::cout << "Has no children!";
        return;
    }
    std::cout << "children: ";
    for (size_t i = 0; i < ls.size(); i++)
    {
        std::cout << (ls)[i];
        if (i < ls.size() - 1)
            std::cout << ", ";
    }
    std::cout << "\n";
}