/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 13:59:20 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/04 13:59:21 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file cub3d.h
 * @brief Main header file for the cub3D project
 * 
 * This file contains the core data structures and type definitions
 * used throughout the cub3D raycasting engine project.
 */

#ifndef CUB3D_H
# define CUB3D_H

# include <stdio.h>
# include "libft.h"

/**
 * @struct s_rgb
 * @brief RGB color representation structure
 * 
 * Stores RGB color values with each component ranging from 0 to 255.
 */
typedef struct s_rgb
{
	int	r;	/**< Red component (0-255) */
	int	g;	/**< Green component (0-255) */
	int	b;	/**< Blue component (0-255) */
}	t_rgb;

/**
 * @struct s_textures
 * @brief Texture and color configuration structure
 * 
 * Contains paths to wall textures and floor/ceiling colors
 * as defined in the .cub configuration file.
 */
typedef struct s_textures
{
	char	*no;		/**< North wall texture path */
	char	*so;		/**< South wall texture path */
	char	*we;		/**< West wall texture path */
	char	*ea;		/**< East wall texture path */
	t_rgb	floor;		/**< Floor color in RGB */
	t_rgb	ceiling;	/**< Ceiling color in RGB */
}	t_textures;

/**
 * @struct s_cub3d
 * @brief Main game state structure
 * 
 * Contains all the essential game data including textures,
 * map layout, and player position information.
 */
typedef struct s_cub3d
{
	t_textures	textures;	/**< Texture and color configuration */
	char		**map;		/**< 2D map array */
	int			map_width;	/**< Map width in characters */
	int			map_height;	/**< Map height in lines */
	int			player_x;	/**< Player X position on map */
	int			player_y;	/**< Player Y position on map */
	char		player_dir;	/**< Player initial direction (N/S/E/W) */
}	t_cub3d;

/**
 * @struct s_readmap_ctx
 * @brief Context structure for map reading operations
 * 
 * Used internally during map file parsing to maintain
 * state across multiple read operations.
 */
typedef struct s_readmap_ctx
{
	char	*line;	/**< Current line being processed */
	int		cap;	/**< Buffer capacity */
	int		len;	/**< Current length */
	char	c;		/**< Current character */
	int		n;		/**< Line number */
	int		stop;	/**< Stop flag */
}	t_readmap_ctx;

#endif
