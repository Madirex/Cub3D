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
#include	 <string.h>
#include <mlx.h>
// void	assign_texture(t_cub3d *cub, char *line);
// int		validate_file_extension(char *filename);
// void	validate_textures(t_cub3d *cub);
// void	init_cub3d(t_cub3d *cub);
// void	read_map(int fd, t_cub3d *cub);

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
#define WIDTH 640
#define HEIGHT 480

// Prototipos de funciones implementadas en raycast.c
void	init_player(t_cub3d *cub);
int		handle_keys(int key, t_cub3d *cub);
int		render_loop(t_cub3d *cub);

// Prototipos de funciones de parseo y validación del repo
void	assign_map(t_cub3d *cub, char *filename);
void	validate_map(t_cub3d *cub);

int	main(int argc, char *argv[])
{
	t_cub3d cub;
	void *mlx;
	void *win;
	void *img;
	char *img_data;
	int bpp, size_line, endian;

	if (argc != 2)
	{
		fprintf(stderr, "Uso: %s <mapa.cub>\n", argv[0]);
		return (1);
	}

	// Estructura cub inicializada a cero
	memset(&cub, 0, sizeof(t_cub3d));

	// Parseo de mapa y validación usando funciones del repo
	assign_map(&cub, argv[1]);
	validate_map(&cub);

	// Inicializa posición y dirección del jugador
	init_player(&cub);

	// Inicialización de MiniLibX
	mlx = mlx_init();
	if (!mlx)
		return (fprintf(stderr, "Error: mlx_init() falló\n"), 1);

	win = mlx_new_window(mlx, WIDTH, HEIGHT, "Cub3D");
	if (!win)
		return (fprintf(stderr, "Error: mlx_new_window() falló\n"), 1);

	img = mlx_new_image(mlx, WIDTH, HEIGHT);
	if (!img)
		return (fprintf(stderr, "Error: mlx_new_image() falló\n"), 1);

	img_data = mlx_get_data_addr(img, &bpp, &size_line, &endian);

	// Guardar referencias en cub para acceso en render_loop y teclas
	cub.mlx = mlx;
	cub.win = win;
	cub.img = img;
	cub.img_data = img_data;
	cub.bpp = bpp;
	cub.size_line = size_line;
	cub.endian = endian;

	// Hook de teclado (key press)
	mlx_hook(win, 2, 1L<<0, handle_keys, &cub);
	// Hook de renderizado: se llama cada frame
	mlx_loop_hook(mlx, render_loop, &cub);

	// Iniciar loop de eventos
	mlx_loop(mlx);

	return (0);
}