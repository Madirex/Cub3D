/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_map.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 13:59:31 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/04 13:59:32 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file parse_map.h
 * @brief Map parsing functions for the Cub3D project
 * 
 * This header contains functions and structures for parsing
 * map data from .cub files.
 */

#ifndef PARSE_MAP_H
# define PARSE_MAP_H

# include "cub3d.h"

/**
 * @brief Context structure for map parsing operations
 */
typedef struct s_map_parse_ctx
{
	char	**temp_map;		/**< Temporary map storage during parsing */
	int		map_lines;		/**< Number of map lines parsed */
	int		map_capacity;	/**< Current capacity of temp_map array */
}	t_map_parse_ctx;

void	assign_map(t_cub3d *cub, char *filename);
int		is_map_line(const char *line, t_cub3d *cub);
void	read_map(int fd, t_cub3d *cub);

#endif
