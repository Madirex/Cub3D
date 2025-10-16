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

    cub->wall_textures = malloc(sizeof(int *) * 4);
    cub->wall_textures[0] = load_xpm_buffer(mlx, cub->textures.no, &w, &h);
    cub->wall_textures[1] = load_xpm_buffer(mlx, cub->textures.so, &w, &h);
    cub->wall_textures[2] = load_xpm_buffer(mlx, cub->textures.we, &w, &h);
    cub->wall_textures[3] = load_xpm_buffer(mlx, cub->textures.ea, &w, &h);
    cub->tex_width = w;
    cub->tex_height = h;

    if (cub->is_bonus)
    {
        if (cub->textures.door_closed != NULL)
        {
            cub->door_textures = malloc(sizeof(int *) * 2);
            if (!cub->door_textures)
                exit(1);
            cub->door_textures[0] = load_xpm_buffer(mlx, cub->textures.door_closed, &w, &h);
            if (cub->textures.door_open != NULL)
                cub->door_textures[1] = load_xpm_buffer(mlx, cub->textures.door_open, &w, &h);
            else
                cub->door_textures[1] = NULL;
            if (!cub->door_textures[0]) {
                fprintf(stderr, "Error: door texture DC path defined but failed to load\n");
                exit(1);
            }
        }
        else
            cub->door_textures = NULL;
    }
    else
        cub->door_textures = NULL;
    for (int i = 0; i < 4; i++)
        if (!cub->wall_textures[i]) {
            fprintf(stderr, "Error: wall texture %d not loaded\n", i);
            exit(1);
        }
}