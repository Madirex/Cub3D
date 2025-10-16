/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_utils.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 17:25:46 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/09 17:25:47 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file map_utils.h
 * @brief Map utility functions for the Cub3D project
 * 
 * This header contains utility functions for map handling,
 * including memory management and map line validation.
 */

#ifndef MAP_UTILS_H
# define MAP_UTILS_H

# include "cub3d.h"

void	free_textures(t_textures *textures);
void	free_map(char **map, int height);
int		is_map_line(const char *line, t_cub3d *cub);

#endif
