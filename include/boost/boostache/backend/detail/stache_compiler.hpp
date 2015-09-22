/**
 *  \file detail/stache_compiler.hpp
 *
 *  Copyright 2014, 2015 Michael Caisse : ciere.com
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef BOOST_BOOSTACHE_BACKEND_DETAIL_STACHE_COMPILER_HPP
#define BOOST_BOOSTACHE_BACKEND_DETAIL_STACHE_COMPILER_HPP

#include <boost/boostache/vm/engine_ast.hpp>
#include <boost/boostache/frontend/stache/ast.hpp>
#include <boost/boostache/backend/detail/stache_compiler.hpp>

#include <boost/algorithm/string/trim.hpp>

namespace boost { namespace boostache { namespace backend { namespace stache_compiler
{
   namespace fe = boost::boostache::frontend;
   namespace detail
   {
      /**
       *  Check if the string simply contains white-space.
       */
      bool is_blank(std::string const & s)
      {
         return( s.find_first_not_of(std::string{" \t\r\n"})
                 ==  std::string::npos );
      }
      
      class stache_visit
      {
      public:
         typedef vm::ast::node result_type;

         stache_visit(std::ostream& out)
            : out(out), output_eol(false)
         {}

         vm::ast::node operator()(fe::stache::ast::undefined) const
         {
            out << "WHOA! we have an undefined" << std::endl;
            return vm::ast::node{};
         }

         vm::ast::node operator()(fe::stache::ast::literal_text const & v) const
         {
            /**
             *  In stache, the literals are trimmed if they are just whitespace.
             */
            // if(is_blank(v))
            // {
            //    return vm::ast::nop{};
            // }
            return vm::ast::literal{v};
         }

         vm::ast::node operator()(fe::stache::ast::blank_text const & v) const
         {
            return vm::ast::literal{v+"blank"};
         }

         vm::ast::node operator()(fe::stache::ast::eol const & v) const
         {
            if(output_eol)
            {
               output_eol = false;
               return vm::ast::literal{v+"eol"};
            }
            
            return vm::ast::nop{};
         }

         vm::ast::node operator()(fe::stache::ast::variable const & v) const
         {
            return vm::ast::render{v.value};
         }

         /**
          *  In stache, the section is a conditional that then loops over
          *  the section body for each data member.
          *
          *  Stache doesn't have an if-then-else structure... just a
          *  conditional that will execute the body if the data element
          *  evaluates to true (or false if marked inverted).
          */
         vm::ast::node operator()(fe::stache::ast::section const & sec) const
         {
            vm::ast::node_list vm_ast;
            for(auto const & node : sec.nodes)
            {
               vm_ast.nodes.push_back(boost::apply_visitor(*this, node));
            }

            vm::ast::for_each section_body;
            section_body.name = sec.name;
            section_body.value = vm_ast;

            vm::ast::if_then_else if_block;
            if_block.condition_.name = sec.name;

            vm::ast::select_context select;
            select.tag = sec.name;
            select.body = section_body;

            if(sec.is_inverted)
            {
               if_block.else_ = select;
            }
            else
            {
               if_block.then_ = select;
            }

            return if_block;
         }

         vm::ast::node operator()(fe::stache::ast::comment const & v) const
         {
            return vm::ast::nop{};
         }         

         vm::ast::node operator()(fe::stache::ast::partial const & v) const
         {
            // TODO: need to implement partials
            return vm::ast::nop{};
         }         

         vm::ast::node operator()(fe::stache::ast::node_list const & nodes) const
         {
            vm::ast::node_list node_list;
            for(auto const & node : nodes)
            {
               node_list.nodes.push_back(boost::apply_visitor(*this, node));
            }
            return node_list;
         }

         vm::ast::node operator()(fe::stache::ast::root const & nodes) const
         {
            vm::ast::node_list node_list;
            for(auto const & node : nodes)
            {
               node_list.nodes.push_back(boost::apply_visitor(*this, node));
            }
            return node_list;
         }

      private:
         std::ostream& out;
         bool output_eol;
      };
   }

   inline vm::ast::node compile(fe::stache::ast::root const & ast)
   {
      detail::stache_visit visit(std::cout);
      return visit(ast);
   }
}}}}

#endif

