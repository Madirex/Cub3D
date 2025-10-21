#include "../includes/cub3d.h"
#include <mlx.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
** Estructura para agrupar info devuelta por mlx_get_data_addr.
*/
typedef struct s_img_info
{
	char	*data;
	int		bpp;
	int		size_line;
	int	endian;
}	t_img_info;

/*
** Copia los píxeles desde el buffer de la imagen a un buffer de int.
*/
static void	copy_image_to_buffer(char *data, int bpp, int *buffer, long count)
{
	long			i;
	int				px;
	unsigned char	*src;

	i = 0;
	while (i < count)
	{
		src = (unsigned char *)data + (i * (bpp / 8));
		px = 0;
		memcpy(&px, src, sizeof(int));
		buffer[i] = px;
		i++;
	}
}

/*
** Helper para devolver NULL imprimiendo un error y destruyendo la imagen si existe.
*/
static int	*load_xpm_err(void *mlx, void *img, char *fmt, char *path)
{
	if (img)
		mlx_destroy_image(mlx, img);
	fprintf(stderr, fmt, path);
	return (NULL);
}

/*
** Carga un XPM en un buffer de int (1 int por pixel).
** Cumple: <= 25 líneas y <= 5 variables locales.
*/
static int	*load_xpm_buffer(void *mlx, char *path, int *width, int *height)
{
	void		*img;
	t_img_info	info;
	int			*buffer;
	long		count;

	img = mlx_xpm_file_to_image(mlx, path, width, height);
	if (!img)
		return (load_xpm_err(mlx, NULL,
				"Error: no se pudo cargar la textura %s\n", path));
	info.data = mlx_get_data_addr(img, &info.bpp, &info.size_line, &info.endian);
	if (!info.data)
		return (load_xpm_err(mlx, img,
				"Error: mlx_get_data_addr falló para %s\n", path));
	count = (long)(*width) * (long)(*height);
	buffer = malloc(count * sizeof(*buffer));
	if (!buffer)
		return (load_xpm_err(mlx, img,
				"Error: malloc falló al reservar buffer para %s\n", path));
	copy_image_to_buffer(info.data, info.bpp, buffer, count);
	mlx_destroy_image(mlx, img);
	return (buffer);
}

/*
** Helper: asigna el array de puertas y comprueba malloc.
*/
static void	init_door_textures_array(t_cub3d *cub)
{
	cub->door_textures = malloc(sizeof(*cub->door_textures) * 2);
	if (!cub->door_textures)
	{
		fprintf(stderr, "Error: malloc falló door_textures\n");
		exit(1);
	}
}

/*
** Helper: carga la textura abierta de la puerta si existe,
** o deja NULL en caso contrario.
*/
static void	set_door_open_texture(t_cub3d *cub, void *mlx, int *tmp_w, int *tmp_h)
{
	int	*buf_open;

	if (cub->textures.door_open == NULL)
	{
		cub->door_textures[1] = NULL;
		return ;
	}
	buf_open = load_xpm_buffer(mlx, cub->textures.door_open, tmp_w, tmp_h);
	cub->door_textures[1] = buf_open;
}

/*
** Carga texturas de puerta (cerrada/abierta) si están definidas.
** Cumple: <= 25 líneas y <= 5 variables locales (tmp_w, tmp_h).
*/
static void	load_doors(t_cub3d *cub, void *mlx, int *w, int *h)
{
	int	tmp_w;
	int	tmp_h;

	if (!cub->is_bonus || cub->textures.door_closed == NULL)
	{
		cub->door_textures = NULL;
		return ;
	}
	init_door_textures_array(cub);
	tmp_w = *w;
	tmp_h = *h;
	cub->door_textures[0] = load_xpm_buffer(mlx,
			cub->textures.door_closed, &tmp_w, &tmp_h);
	set_door_open_texture(cub, mlx, &tmp_w, &tmp_h);
	if (!cub->door_textures[0])
	{
		fprintf(stderr, "Error: door texture definida pero fallo al cargar\n");
		exit(1);
	}
}

/*
** Carga las 4 texturas de pared y devuelve ancho/alto por referencia.
*/
static void	load_walls(t_cub3d *cub, void *mlx, int *w, int *h)
{
	int	tmp_w;
	int	tmp_h;

	tmp_w = 0;
	tmp_h = 0;
	cub->wall_textures = malloc(sizeof(*cub->wall_textures) * 4);
	if (!cub->wall_textures)
	{
		fprintf(stderr, "Error: malloc falló wall_textures\n");
		exit(1);
	}
	cub->wall_textures[0] = load_xpm_buffer(mlx, cub->textures.no, &tmp_w, &tmp_h);
	cub->wall_textures[1] = load_xpm_buffer(mlx, cub->textures.so, &tmp_w, &tmp_h);
	cub->wall_textures[2] = load_xpm_buffer(mlx, cub->textures.we, &tmp_w, &tmp_h);
	cub->wall_textures[3] = load_xpm_buffer(mlx, cub->textures.ea, &tmp_w, &tmp_h);
	*w = tmp_w;
	*h = tmp_h;
}

/*
** Verifica que las texturas de pared se hayan cargado correctamente.
*/
static void	verify_wall_textures(t_cub3d *cub)
{
	int	i;

	i = 0;
	while (i < 4)
	{
		if (!cub->wall_textures[i])
		{
			fprintf(stderr, "Error: wall texture %d not loaded\n", i);
			exit(1);
		}
		i++;
	}
}

/*
** Interfaz pública: carga las texturas y configura cub->tex_width/tex_height.
*/
void	load_wall_textures(t_cub3d *cub, void *mlx)
{
	int	w;
	int	h;

	w = 0;
	h = 0;
	load_walls(cub, mlx, &w, &h);
	cub->tex_width = w;
	cub->tex_height = h;
	load_doors(cub, mlx, &w, &h);
	verify_wall_textures(cub);
}