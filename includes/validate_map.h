/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate_map.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 17:25:56 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/09 17:25:57 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file validate_map.h
 * @brief Map validation functions for the Cub3D project
 * 
 * This header contains functions for validating map structure
 * and ensuring it meets the requirements for the raycasting engine.
 */

#ifndef VALIDATE_MAP_H
# define VALIDATE_MAP_H

# include "cub3d.h"

void	validate_map(t_cub3d *cub);
void init_cub3d(t_cub3d *cub, int is_bonus);
void validate_textures(t_cub3d *cub);

#endif
