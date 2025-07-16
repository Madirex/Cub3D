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

#ifndef PARSE_MAP_H
# define PARSE_MAP_H

# include "cub3d.h"

void	assign_map(t_cub3d *cub, char *filename);
int		is_map_line(const char *line);

typedef struct s_map_parse_ctx
{
	char	**temp_map;
	int		map_lines;
	int		map_capacity;
}	t_map_parse_ctx;

#endif
