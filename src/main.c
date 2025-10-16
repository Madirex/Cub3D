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
#include <string.h>
#include <mlx.h>

#define WIDTH 1248
#define HEIGHT 960
#define IS_BONUS 1

// Prototipos de funciones implementadas en raycast.c
void	init_player(t_cub3d *cub);
int		handle_key_press(int key, t_cub3d *cub);
int		handle_key_release(int key, t_cub3d *cub);
int		render_loop(t_cub3d *cub);

// Prototipos de funciones de parseo y validación del repo
void	assign_map(t_cub3d *cub, char *filename);
void	validate_map(t_cub3d *cub);

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

/* Funcion de salida y limpieza del programa */

int	exit_program(t_cub3d *cub)
{
	if (!cub)
		exit(0);
	if (cub->img && cub->mlx)
		mlx_destroy_image(cub->mlx, cub->img);
	if (cub->win && cub->mlx)
		mlx_destroy_window(cub->mlx, cub->win);
	free_textures(&cub->textures);
	if (cub->map)
		free_map(cub->map, cub->map_height);
	exit(0);
	return (0);
}

int	main(int argc, char *argv[])
{
	(void)	argc;
	int		fd;
	t_cub3d	cub;
	void	*mlx, *win, *img;
	char	*img_data;
	int		bpp, size_line, endian;

	init_cub3d(&cub, IS_BONUS);
	fd = open(argv[1], O_RDONLY);
	read_map(fd, &cub);
	close(fd);
	validate_textures(&cub);
	assign_map(&cub, argv[1]);
	validate_map(&cub);
	init_player(&cub);
	mlx = mlx_init();
	win = mlx_new_window(mlx, WIDTH, HEIGHT, "Cub3D");
	img = mlx_new_image(mlx, WIDTH, HEIGHT);
	img_data = mlx_get_data_addr(img, &bpp, &size_line, &endian);
	cub.mlx = mlx;
	cub.win = win;
	cub.img = img;
	cub.img_data = img_data;
	cub.bpp = bpp;
	cub.size_line = size_line;
	cub.endian = endian;
	load_wall_textures(&cub, mlx);
	mlx_hook(cub.win, 2, 1L << 0, handle_key_press, &cub);
	mlx_hook(cub.win, 3, 1L << 1, handle_key_release, &cub);
	mlx_hook(cub.win, 17, 0, exit_program, &cub);
	mlx_loop_hook(mlx, render_loop, &cub);
	mlx_loop(mlx);
	free_textures(&cub.textures);
	if (cub.map)
		free_map(cub.map, cub.map_height);
	return (0);
}
