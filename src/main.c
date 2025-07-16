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

/**
 * @file main.c
 * @brief Main entry point for the Cub3D project
 * 
 * This file contains the main function that orchestrates the parsing,
 * validation, and initialization of the Cub3D raycasting engine.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "../includes/cub3d.h"
#include "../includes/parse_textures.h"
#include "../includes/parse_map.h"
#include "../includes/debug.h"
#include "../includes/map_utils.h"
#include "../includes/validate_map.h"
#include "../includes/utils.h"

void	assign_texture(t_cub3d *cub, char *line);
int		validate_file_extension(char *filename);
void	validate_textures(t_cub3d *cub);
void	init_cub3d(t_cub3d *cub);
void	read_map(int fd, t_cub3d *cub);

/**
 * @brief Main entry point of the Cub3D program
 * 
 * Handles command line arguments, file validation, parsing,
 * and cleanup of resources.
 * 
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return 0 on success, 1 on error
 */
int	main(int argc, char *argv[])
{
	int		fd;
	t_cub3d	cub;

	init_cub3d(&cub);
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
	validate_map(&cub);
	print_map_debug(&cub);
	free_textures(&cub.textures);
	if (cub.map)
		free_map(cub.map, cub.map_height);
	return (0);
}
