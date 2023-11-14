/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mschaub <mschaub@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 10:54:41 by mschaub           #+#    #+#             */
/*   Updated: 2023/11/10 15:27:35 by mschaub          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

enum Method {
    GET,
    POST,
    DELETE,
    NONE
};

class Request {
    public:
        Request();
        ~Request();
    private:
        Method _method;
};
