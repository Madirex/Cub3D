/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 13:58:20 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/04 13:58:22 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "../includes/cub3d.h"
#include "../includes/parse_textures.h"
#include "../includes/parse_map.h"
#include "../includes/debug.h"
#include "../includes/map_utils.h"
#include "../includes/utils.h"

int	validate_file_extension(char *filename)
{
	int	len;

	len = ft_strlen(filename);
	if (len < 4)
		return (0);
	if (ft_strncmp(filename + len - 4, ".cub", 4) != 0)
		return (0);
	return (1);
}

void	validate_textures(t_cub3d *cub)
{
	if (!cub->textures.no || !cub->textures.so || !cub->textures.we || !cub->textures.ea
		|| cub->textures.floor.r == -1 || cub->textures.floor.g == -1 || cub->textures.floor.b == -1
		|| cub->textures.ceiling.r == -1 || cub->textures.ceiling.g == -1 || cub->textures.ceiling.b == -1)
		ft_error("Missing required fields in .cub file", cub, NULL);
}

void	read_map(int fd, t_cub3d *cub)
{
	char	*line;
	int		cap;
	int		len;
	char	c;
	int		n;

	cap = 128;
	line = (char *)malloc(cap);
	len = 0;
	if (!line)
		ft_error("Memory allocation failed for line", cub, NULL);
	while ((n = read(fd, &c, 1)) > 0)
	{
		if (c == '\n')
		{
			line[len] = '\0';
			if (line[0] != '\0')
			{
				if (is_texture_line(line, "NO ")
					|| is_texture_line(line, "SO ")
					|| is_texture_line(line, "WE ")
					|| is_texture_line(line, "EA "))
				{
					assign_texture(cub, line);
				}
				else if (is_color_line(line, 'F'))
					assign_color(&cub->textures.floor, line);
				else if (is_color_line(line, 'C'))
					assign_color(&cub->textures.ceiling, line);
			}
			len = 0;
			line[0] = '\0';
			continue ;
		}
		line[len++] = c;
		if (len + 1 >= cap)
		{
			cap *= 2;
			char *tmp = (char *)malloc(cap);
			if (!tmp)
				ft_error("Memory allocation failed for line expansion", cub, line);
			ft_memcpy(tmp, line, len);
			free(line);
			line = tmp;
		}
	}
	free(line);
	print_textures_debug(cub);
	validate_textures(cub);
}

int	main(int argc, char *argv[])
{
	int		fd;
	t_cub3d	cub;

	ft_memset(&cub, 0, sizeof(t_cub3d));
	cub.textures.floor.r = -1;
	cub.textures.floor.g = -1;
	cub.textures.floor.b = -1;
	cub.textures.ceiling.r = -1;
	cub.textures.ceiling.g = -1;
	cub.textures.ceiling.b = -1;
	if (argc != 2)
	{
		fprintf(stderr, "Use: %s <map.cub>\n", argv[0]);
		return (1);
	}
	if (!validate_file_extension(argv[1]))
		ft_error("Invalid file extension. Please use a .cub file", NULL, NULL);
	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
		ft_error("Cannot open file", NULL, NULL);
	read_map(fd, &cub);
	close(fd);
	validate_textures(&cub);
	assign_map(&cub, argv[1]);
	print_map_debug(&cub);
	free_textures(&cub.textures);
	if (cub.map)
		free_map(cub.map, cub.map_height);
	return (0);
}
