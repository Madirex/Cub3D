/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 13:58:48 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/09 19:59:14 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include "../../includes/cub3d.h"
#include "../../includes/map_utils.h"

static void	safe_exit(t_cub3d *cub, char *line)
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

void	ft_error(char *message, t_cub3d *cub, char *line)
{
	printf("Error\n");
	if (message)
		printf("%s\n", message);
	safe_exit(cub, line);
}

int	is_whitespace_only(const char *line)
{
	int	i;

	if (!line)
		return (1);
	i = 0;
	while (line[i])
	{
		if (line[i] != ' ' && line[i] != '\t'
			&& line[i] != '\n' && line[i] != '\r')
			return (0);
		i++;
	}
	return (1);
}

void	init_cub3d(t_cub3d *cub)
{
	ft_memset(cub, 0, sizeof(t_cub3d));
	cub->textures.floor.r = -1;
	cub->textures.floor.g = -1;
	cub->textures.floor.b = -1;
	cub->textures.ceiling.r = -1;
	cub->textures.ceiling.g = -1;
	cub->textures.ceiling.b = -1;
}
