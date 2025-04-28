/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andreas <andreas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 18:33:31 by adeters           #+#    #+#             */
/*   Updated: 2025/04/29 01:31:59 by andreas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

int	check_arg(t_data *data, char *input, int *value, bool is_time)
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
	if (is_time && INT_MAX / 1000 < *value)
		return (p_err_arg(ERR_OVERFLOW, input));
	if (is_time)
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
	if (check_arg(data, av[1], &data->nbp, false))
		flag = ERR_INV_ARG;
	if (data->nbp == 0)
		return (data->error = ERR_NO_PHILO);
	if (check_arg(data, av[2], &data->ttd, true))
		flag = ERR_INV_ARG;
	if (check_arg(data, av[3], &data->tte, true))
		flag = ERR_INV_ARG;
	if (check_arg(data, av[4], &data->tts, true))
		flag = ERR_INV_ARG;
	if (ac == 6 && check_arg(data, av[5], &data->nbte, false))
		flag = ERR_INV_ARG;
	else if (ac != 6)
		data->nbte = -1;
	return (data->error = flag, flag);
}
