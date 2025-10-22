/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycast_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migonzal <migonzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 12:40:00 by skyce11           #+#    #+#             */
/*   Updated: 2025/10/22 15:31:11 by migonzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d_render.h"
#include <stdlib.h>

/* Return wall texture index based on side and ray direction */
int	get_wall_texture(int side, double ray_dir_x, double ray_dir_y)
{
	if (side == 0)
	{
		if (ray_dir_x > 0)
			return (2);
		return (3);
	}
	if (ray_dir_y > 0)
		return (1);
	return (0);
}

/* Convert rgb components to packed int */
int	rgb_to_int(int r, int g, int b)
{
	return (((r & 0xFF) << 16)
		| ((g & 0xFF) << 8)
		| (b & 0xFF));
}

/* Pixel helper: uses t_img to avoid passing multiple args */
void	draw_pixel(t_img *img, int x, int y, int color)
{
	unsigned int	*pixel;
	unsigned char	*addr;

	if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
		return ;
	addr = (unsigned char *)img->data;
	addr += y * img->size_line;
	addr += x * img->bytes_per_pixel;
	pixel = (unsigned int *)addr;
	*pixel = (unsigned int)color;
}

/*
** If character is floor/empty/player
*/
int	is_floor(char c)
{
	return (c == '0' || c == 'N' || c == 'S'
		|| c == 'E' || c == 'W' || c == 'O');
}
