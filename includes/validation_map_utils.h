/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validation_map_utils.h                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 04:35:06 by anmateo-          #+#    #+#             */
/*   Updated: 2025/10/22 04:35:29 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VALIDATION_MAP_UTILS_H
# define VALIDATION_MAP_UTILS_H

void	flood_fill(t_cub3d *cub, int **visited, int x, int y);
void	handle_visited_allocation_failure(
			int **visited, int i, t_cub3d *cub);
int		is_valid_position(t_cub3d *cub, int x, int y);
int		check_door_orientation(t_cub3d *cub, int x, int y);

#endif