/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 13:58:48 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/04 13:58:49 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include "../../includes/cub3d.h"
#include "../../includes/map_utils.h"

void	safe_exit(t_cub3d *cub, char *line)
{
	if (cub)
	{
		free_textures(&cub->textures);
		if (cub->map)
			free_map(cub->map, cub->map_height);
	}
	if (line)
		free(line);
	exit(1);
}