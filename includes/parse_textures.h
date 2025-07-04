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

#ifndef PARSE_TEXTURES_H
#define PARSE_TEXTURES_H

#include "cub3d.h"

int		is_texture_line(const char *line, const char *id);
int		is_color_line(const char *line, char id);
void	assign_texture(t_cub3d *cub, char *line);
void	assign_color(t_rgb *color, char *line, const char *type);

#endif
