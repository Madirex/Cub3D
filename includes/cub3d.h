/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 13:59:20 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/04 13:59:21 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file cub3d.h
 * @brief Main header file for the cub3D project
 * 
 * This file contains the core data structures and type definitions
 * used throughout the cub3D raycasting engine project.
 */

#ifndef CUB3D_H
# define CUB3D_H

# include <stdio.h>
# include "libft.h"

/**
 * @struct s_rgb
 * @brief RGB color representation structure
 * 
 * Stores RGB color values with each component ranging from 0 to 255.
 */
typedef struct s_rgb
{
	int	r;	/**< Red component (0-255) */
	int	g;	/**< Green component (0-255) */
	int	b;	/**< Blue component (0-255) */
}	t_rgb;

/**
 * @struct s_textures
 * @brief Texture and color configuration structure
 * 
 * Contains paths to wall textures and floor/ceiling colors
 * as defined in the .cub configuration file.
 */
typedef struct s_textures
{
	char	*no;		/**< North wall texture path */
	char	*so;		/**< South wall texture path */
	char	*we;		/**< West wall texture path */
	char	*ea;		/**< East wall texture path */
	char	*door_closed;
	char	*door_open;
	t_rgb	floor;		/**< Floor color in RGB */
	t_rgb	ceiling;	/**< Ceiling color in RGB */
}	t_textures;

/**
 * @struct s_cub3d
 * @brief Main game state structure
 * 
 * Contains all the essential game data including textures,
 * map layout, and player position information.
 */
typedef struct s_cub3d
{
	t_textures	textures;	/**< Texture and color configuration */
	char		**map;		/**< 2D map array */
	int			map_width;	/**< Map width in characters */
	int			map_height;	/**< Map height in lines */
	int			player_x;	/**< Player X position on map */
	int			player_y;	/**< Player Y position on map */
	char		player_dir;	/**< Player initial direction (N/S/E/W) */
	int			**wall_textures; // [4][tex_width*tex_height]
	int			**door_textures; // [1][tex_width*tex_height]
	int			tex_width;
	int			tex_height;
	int			is_bonus;
	// --- Añadidos para raycasting y MLX ---
	int			is_moving_forward;
	int			is_moving_backward;
	int			is_rotating_left;
	int			is_rotating_right;
	double		pos_x;		// Posición real X (con decimales)
	double		pos_y;		// Posición real Y
	double		dir_x;		// Dirección del jugador X
	double		dir_y;		// Dirección del jugador Y
	double		plane_x;	// Plano de cámara X
	double		plane_y;	// Plano de cámara Y
	void		*mlx;		// Puntero a la instancia MLX
	void		*win;		// Puntero a la ventana MLX
	void		*img;		// Imagen para dibujar
	char		*img_data;	// Buffer de datos de píxel de la imagen
	int			bpp;		// Bits por píxel
	int			size_line;	// Bytes por línea de la imagen
	int			endian;		// Endianess de la imagen
	// Delta Time
	double		time_frame;
	long		time_prev;
	// Mouse rotation
	int			last_mouse_x;
}	t_cub3d;

/**
 * @struct s_readmap_ctx
 * @brief Context structure for map reading operations
 * 
 * Used internally during map file parsing to maintain
 * state across multiple read operations.
 */
typedef struct s_readmap_ctx
{
	char	*line;	/**< Current line being processed */
	int		cap;	/**< Buffer capacity */
	int		len;	/**< Current length */
	char	c;		/**< Current character */
	int		n;		/**< Line number */
	int		stop;	/**< Stop flag */
}	t_readmap_ctx;

int		open_and_parse_map(t_cub3d *cub, char *map_path);
void	init_mlx_and_game(t_cub3d *cub);
void	setup_hooks_and_run(t_cub3d *cub);
void	free_game_resources(t_cub3d *cub);

typedef struct s_img
{
	char	*data;
	int		size_line;
	int		bpp;
	int		bytes_per_pixel;
}				t_img;



//Raycast
int		exit_program(t_cub3d *cub);
void	ft_error(char *message, t_cub3d *cub, char *line);
void	draw_door_prompt(t_cub3d *cub);
void	draw_minimap(t_cub3d *cub, t_img *img);
void	raycast_render(t_cub3d *cub, t_img *img);
int		render_loop(t_cub3d *cub);
int		handle_key_press(int key, t_cub3d *cub);
int		handle_key_release(int key, t_cub3d *cub);
int		handle_mouse_move(int x, int y, t_cub3d *cub);
void	init_player(t_cub3d *cub);
void	perform_movements(t_cub3d *cub);
void	rotate_player(t_cub3d *cub, double angle);
long	fn_get_time_in_ms(void);

#endif
