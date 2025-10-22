/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   xpm_util.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migonzal <migonzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 12:48:38 by skyce11           #+#    #+#             */
/*   Updated: 2025/10/22 16:28:17 by migonzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d_render.h"
#include <mlx.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
** Copy pixels from mlx buffer to an int buffer (1 int per pixel).
*/
void	copy_image_to_buffer(char *data, int bpp, int *buffer, long count)
{
	long			i;
	unsigned char	*src;

	i = 0;
	while (i < count)
	{
		src = (unsigned char *)data + (i * (bpp / 8));
		ft_memcpy(&buffer[i], src, sizeof(int));
		i++;
	}
}

/*
** Print an error and destroy image if needed, then return NULL.
*/
int	*load_xpm_err(void *mlx, void *img, const char *fmt, const char *path)
{
	if (img)
		mlx_destroy_image(mlx, img);
	fprintf(stderr, fmt, path);
	return (NULL);
}

/*
** Load an XPM into a newly allocated int buffer.
*/
int	*load_xpm_buffer(void *mlx, const char *path, int *width, int *height)
{
	void			*img;
	t_img_info		info;
	int				*buffer;
	long			count;

	img = mlx_xpm_file_to_image(mlx, (char *)path, width, height);
	if (!img)
		return (load_xpm_err(mlx, NULL,
				"Error: no se pudo cargar la textura %s\n", (char *)path));
	info.data = mlx_get_data_addr(img, &info.bpp,
			&info.size_line, &info.endian);
	if (!info.data)
		return (load_xpm_err(mlx, img,
				"Error: mlx_get_data_addr falló para %s\n", (char *)path));
	count = (long)(*width) *(long)(*height);
	buffer = malloc(count * sizeof(*buffer));
	if (!buffer)
		return (load_xpm_err(mlx, img,
				"Error: malloc falló al reservar buffer para %s\n",
				(char *)path));
	copy_image_to_buffer(info.data, info.bpp, buffer, count);
	mlx_destroy_image(mlx, img);
	return (buffer);
}
