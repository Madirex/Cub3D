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
#define IS_BONUS 0

void	init_player(t_cub3d *cub);
int		handle_key_press(int key, t_cub3d *cub);
int		handle_key_release(int key, t_cub3d *cub);
int		render_loop(t_cub3d *cub);
int		handle_mouse_move(int x, int y, t_cub3d *cub);
void	assign_map(t_cub3d *cub, char *filename);
void	validate_map(t_cub3d *cub);
void	safe_exit(t_cub3d *cub, char *line, int exit_code);

/**
 * @brief Opens and parses the map file
 * 
 * Opens the specified map file, reads and validates
 * its contents, and initializes the player position.
 * 
 * @param cub Pointer to the main Cub3D structure
 * @param map_path Path to the map file
 * @return File descriptor of the opened map file
 */
int	open_and_parse_map(t_cub3d *cub, char *map_path)
{
	int	fd;

	fd = open(map_path, O_RDONLY);
	read_map(fd, cub);
	validate_textures(cub);
	assign_map(cub, map_path);
	validate_map(cub);
	init_player(cub);
	print_map_debug(cub);
	return (fd);
}

/**
 * @brief Retrieves image data from the MLX image
 * 
 * Populates the Cub3D structure with image data pointers
 * and properties such as bits per pixel, size line, and endianess.
 * 
 * @param cub Pointer to the main Cub3D structure
 */
void	get_image_data(t_cub3d *cub)
{
	char	*img_data;
	int		bpp;
	int		size_line;
	int		endian;

	img_data = mlx_get_data_addr(cub->img, &bpp, &size_line, &endian);
	cub->img_data = img_data;
	cub->bpp = bpp;
	cub->size_line = size_line;
	cub->endian = endian;
}

/**
 * @brief Initializes MLX pointers for window and image
 * 
 * Sets up the MLX instance, creates a new window,
 * and initializes the image buffer.
 * 
 * @param cub Pointer to the main Cub3D structure
 * @param mlx Pointer to the MLX instance
 */
void	init_mlx_pointers(t_cub3d *cub, void *mlx)
{
	void	*win;
	void	*img;

	cub->mlx = mlx;
	win = mlx_new_window(mlx, WIDTH, HEIGHT, "Cub3D");
	img = mlx_new_image(mlx, WIDTH, HEIGHT);
	cub->win = win;
	cub->img = img;
}

/**
 * @brief Sets up event hooks and starts the main loop
 * 
 * Configures key press/release, window close, mouse movement,
 * and rendering loop hooks, then enters the MLX main loop.
 * 
 * @param cub Pointer to the main Cub3D structure
 */
void	setup_hooks_and_run(t_cub3d *cub)
{
	mlx_hook(cub->win, 2, 1L << 0, handle_key_press, cub);
	mlx_hook(cub->win, 3, 1L << 1, handle_key_release, cub);
	mlx_hook(cub->win, 17, 0, exit_program, cub);
	if (cub->is_bonus)
		mlx_hook(cub->win, 6, 1L << 6, handle_mouse_move, cub);
	mlx_loop_hook(cub->mlx, render_loop, cub);
	mlx_loop(cub->mlx);
}

/**
 * @brief Main entry point for the Cub3D application
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit status
 */
int	main(int argc, char *argv[])
{
	int			fd;
	t_cub3d		cub;
	void		*mlx;

	if (argc != 2)
		return (1);
	(void) argc;
	init_cub3d(&cub, IS_BONUS);
	fd = open_and_parse_map(&cub, argv[1]);
	close(fd);
	mlx = mlx_init();
	init_mlx_pointers(&cub, mlx);
	get_image_data(&cub);
	load_wall_textures(&cub, cub.mlx);
	cub.last_mouse_x = WIDTH / 2;
	if (cub.is_bonus)
		mlx_mouse_hide(cub.mlx, cub.win);
	setup_hooks_and_run(&cub);
	safe_exit(&cub, NULL, 0);
	return (0);
}
