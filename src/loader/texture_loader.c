#include "../includes/cub3d.h"
#include <mlx.h>
#include <stdlib.h>
#include <stdio.h>

int *load_xpm_buffer(void *mlx, char *path, int *width, int *height)
{
    void *img;
    int bpp, size_line, endian;
    char *data;
    int *buffer;

    img = mlx_xpm_file_to_image(mlx, path, width, height);
    if (!img) {
        fprintf(stderr, "Error: No se pudo cargar la textura %s\n", path);
        return NULL;
    }
    data = mlx_get_data_addr(img, &bpp, &size_line, &endian);

    buffer = malloc((*width) * (*height) * sizeof(int));
    if (!buffer)
        return NULL;
    for (int i = 0; i < (*width) * (*height); i++)
        buffer[i] = ((int *)data)[i];
    mlx_destroy_image(mlx, img);
    return buffer;
}

void load_wall_textures(t_cub3d *cub, void *mlx)
{
    int w, h;
    int texture_count;

    texture_count = 4;
    if (cub->is_bonus)
        texture_count += 2;
    cub->wall_textures = malloc(sizeof(int *) * texture_count);
    cub->wall_textures[0] = load_xpm_buffer(mlx, cub->textures.no, &w, &h);
    cub->wall_textures[1] = load_xpm_buffer(mlx, cub->textures.so, &w, &h);
    cub->wall_textures[2] = load_xpm_buffer(mlx, cub->textures.we, &w, &h);
    cub->wall_textures[3] = load_xpm_buffer(mlx, cub->textures.ea, &w, &h);
    if (cub->is_bonus) {
        cub->wall_textures[4] = load_xpm_buffer(mlx, cub->textures.door_closed, &w, &h);
        cub->wall_textures[5] = load_xpm_buffer(mlx, cub->textures.door_open, &w, &h);
    }
    cub->tex_width = w;
    cub->tex_height = h;

if (cub->is_bonus)
    {
        cub->door_textures = malloc(sizeof(int *) * 2); 
        cub->door_textures[0] = load_xpm_buffer(mlx, cub->textures.door_closed, &w, &h);
        cub->door_textures[1] = load_xpm_buffer(mlx, cub->textures.door_open, &w, &h);
        if (!cub->door_textures[0] || !cub->door_textures[1]) {
            fprintf(stderr, "Error: door texture not loaded\n");
            exit(1);
        }
    }
    else
    {
        cub->door_textures = NULL;
    }
    for (int i = 0; i < 4; i++)
        if (!cub->wall_textures[i]) {
            fprintf(stderr, "Error: wall texture %d not loaded\n", i);
            exit(1);
        }
}