/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adeters <adeters@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 18:33:31 by adeters           #+#    #+#             */
/*   Updated: 2025/04/07 16:46:39 by adeters          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

int	check_arg(t_data *data, char *input, int *value)
{
	int	error;

	error = check_overflow(input);
	if (error == 1)
		return (data->error = p_err_arg(ERR_OVERFLOW, input), 1);
	else if (error == -1)
		return (data->error = p_err_arg(ERR_UNDERFLOW, input), 1);
	else if (error == -2)
		return (data->error = p_err_arg(ERR_INV_ARG, input), 1);
	*value = ft_atoi(input);
	if (INT_MAX / 1000 < *value)
		return (p_err_arg(ERR_OVERFLOW, input));
	*value = *value * 1000;
	if (*value < 0)
		return (data->error = p_err_arg(ERR_NEG_ARG, input), 1);
	return (0);
}

int	parse_args(t_data *data, int ac, char **av)
{
	int	flag;

	flag = 0;
	if (ac < 5 || ac > 6)
		return (data->error = ERR_USAGE);
	if (check_arg(data, av[1], &data->nbp))
		flag = ERR_INV_ARG;
	if (check_arg(data, av[2], &data->ttd))
		flag = ERR_INV_ARG;
	if (check_arg(data, av[3], &data->tte))
		flag = ERR_INV_ARG;
	if (check_arg(data, av[4], &data->tts))
		flag = ERR_INV_ARG;
	if (ac > 5 && check_arg(data, av[5], &data->nbte))
		flag = ERR_INV_ARG;
	else
		data->nbte = -1;
	return (data->error = flag, flag);
}

int	init_prog(t_data *data, int ac, char **av)
{
	if (parse_args(data, ac, av))
		return (data->error);
	data->is_kil = false;
	return (0);
}
