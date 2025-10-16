/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_textures.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 13:59:41 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/04 13:59:42 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file parse_textures.h
 * @brief Texture parsing functions for the Cub3D project
 * 
 * This header contains functions for parsing texture paths
 * and color information from .cub files.
 */

#ifndef PARSE_TEXTURES_H
# define PARSE_TEXTURES_H

# include "cub3d.h"

int		is_texture_line(const char *line, const char *id);
int		is_color_line(const char *line, char id);
void	assign_color(t_rgb *color, char *line);
int *load_xpm_buffer(void *mlx, char *path, int *width, int *height);
void load_wall_textures(t_cub3d *cub, void *mlx);

#endif
