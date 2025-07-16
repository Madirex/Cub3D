/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 13:59:24 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/04 13:59:25 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file debug.h
 * @brief Debug utilities for the Cub3D project
 * 
 * This header contains function declarations for debugging purposes,
 * including functions to print texture and map information.
 */

#ifndef DEBUG_H
# define DEBUG_H

# include "../../includes/cub3d.h"

void	print_textures_debug(t_cub3d *cub);
void	print_map_debug(t_cub3d *cub);

#endif
