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

void	init_player(t_cub3d *cub);
int		handle_key_press(int key, t_cub3d *cub);
int		handle_key_release(int key, t_cub3d *cub);
int		render_loop(t_cub3d *cub);
int		handle_mouse_move(int x, int y, t_cub3d *cub);
void	assign_map(t_cub3d *cub, char *filename);
void	validate_map(t_cub3d *cub);

/**
 * @brief Cleans up resources and exits the program
 * * @param cub Pointer to the main Cub3D structure
 * @return 0 (never returns, exits the program)
 */
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

int open_and_parse_map(t_cub3d *cub, char *map_path)
{
    int fd;

    fd = open(map_path, O_RDONLY);
    read_map(fd, cub);
    validate_textures(cub);
    assign_map(cub, map_path);
    validate_map(cub);
    init_player(cub);
    print_map_debug(cub);
    return (fd);
}

void init_mlx_and_game(t_cub3d *cub)
{
    void    *mlx;
    void    *win;
    void    *img;
    char    *img_data;
    int     bpp;
    int     size_line;
    int     endian;

    mlx = mlx_init();
    win = mlx_new_window(mlx, WIDTH, HEIGHT, "Cub3D");
    img = mlx_new_image(mlx, WIDTH, HEIGHT);
    img_data = mlx_get_data_addr(img, &bpp, &size_line, &endian);

    cub->mlx = mlx;
    cub->win = win;
    cub->img = img;
    cub->img_data = img_data;
    cub->bpp = bpp;
    cub->size_line = size_line;
    cub->endian = endian;

    load_wall_textures(cub, mlx);
    cub->last_mouse_x = WIDTH / 2;
    mlx_mouse_hide(cub->mlx, cub->win);
}

void setup_hooks_and_run(t_cub3d *cub)
{
    mlx_hook(cub->win, 2, 1L << 0, handle_key_press, cub);
    mlx_hook(cub->win, 3, 1L << 1, handle_key_release, cub);
    mlx_hook(cub->win, 17, 0, exit_program, cub);
    mlx_hook(cub->win, 6, 1L << 6, handle_mouse_move, cub);
    
    mlx_loop_hook(cub->mlx, render_loop, cub);
    mlx_loop(cub->mlx);
}

void free_game_resources(t_cub3d *cub)
{
	free_textures(&cub->textures);
	if (cub->map)
		free_map(cub->map, cub->map_height);
}

int	main(int argc, char *argv[])
{
	int			fd;
	t_cub3d		cub;

	if (argc != 2)
		return (1);
	(void) argc;
	init_cub3d(&cub, IS_BONUS);
	fd = open_and_parse_map(&cub, argv[1]);
	close(fd);
	init_mlx_and_game(&cub);
	setup_hooks_and_run(&cub);
	free_game_resources(&cub);
	return (0);
}