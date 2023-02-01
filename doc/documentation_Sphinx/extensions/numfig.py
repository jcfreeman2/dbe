#
# author: https://bitbucket.org/arjones6/sphinx-numfig/wiki/Home
#
# numfig
#
# Numfig provides the following configuration variables:
# variable	description
# number_figures	If True, will modify figure captions in html output to 
# include a figure 
# number.
# figure_caption_prefix	Set to a string that will be included before the figure 
# number for # html figure captions. Defaults to "Figure"

# Numfig also defines the following roles, which can be used similarly to 
# the :ref: role.
#
#
# role	description
#
# :num:	Provides a link to the referenced figure, with the text of the link 
# being the figure number instead of the figure caption. 
# To include more than just the figure number in the link, use the role as 
# follows :num:`figure #reftarget`. 
# Provided that reftarget refers to the 3rd figure, this would be replaced with
# a link that says "figure 3".
#
# :page:	The reference will be replaced by the page number the referenced 
# figure occurs on. This only works with latex output and is ignored for other 
# outputs.
#
# Example usage:
#
# See :num:`figure #example-figtwo` on page :page:`example-fig` for the figure 
# labeled :ref:`example_fig`.
#
# .. _example_figTwo:
#
# .. figure:: example_figure.png
#
#    Example figure
#
# PLEASE PAY ATTENTION to the syntax: for some reason, when using :num:, 
# you have to substitute the underscores "_" with the hyphen "-" 
# and the capital letters with the small ones, 
# otherwise the ref labels are not found.
#


from docutils.nodes import figure, caption, Text, reference, raw, SkipNode, Element
from sphinx.roles import XRefRole

import pprint



def setup(app):
    app.add_config_value('number_figures', True, True)
    app.add_config_value('figure_caption_prefix', "Figure", True)

    app.add_node(page_ref,
                 text=(skip_page_ref, depart_page_ref),
                 html=(skip_page_ref, depart_page_ref),
                 latex=(visit_page_ref, depart_page_ref))

    app.add_role('page', XRefRole(nodeclass=page_ref))

    app.add_node(num_ref,
                 text=(visit_num_ref, depart_num_ref),
                 html=(visit_num_ref, depart_num_ref),
                 latex=(latex_visit_num_ref, latex_depart_num_ref))

    app.add_role('num', XRefRole(nodeclass=num_ref))

    app.connect('doctree-resolved', number_figure_nodes)

# Element classes
class page_ref(reference):
    pass

class num_ref(reference):
    pass

def skip_page_ref(self, node):
    raise SkipNode

def visit_page_ref(self, node):
    self.body.append("\\pageref{%s:%s}" % (node['refdoc'], node['reftarget']))
    raise SkipNode

def depart_page_ref(self, node):
    pass

def visit_num_ref(self, node):
    pass

def depart_num_ref(self, node):
    pass

def latex_visit_num_ref(self, node):
    fields = node['reftarget'].split('#')
    if len(fields) > 1:
        label, target = fields
        ref_link = '%s:%s' % (node['refdoc'], target)
        latex = "\\hyperref[%s]{%s \\ref*{%s}}" % (ref_link, label, ref_link)
        self.body.append(latex)
    else:
        self.body.append('\\ref{%s:%s}' % (node['refdoc'], fields[0]))
        
    raise SkipNode

def latex_depart_num_ref(self, node):
    pass

def number_figure_nodes(app, doctree, docname):
    env = app.builder.env
    # first generate figure numbers for each figure
    i = 1
    figids = {}
    
    # print "\nDEBUG - doctree:", doctree
    
    # looping over 'reference' objects for DEBUG (rbianchi)
    #for ref_info in doctree.traverse(reference):
    #    print "\nref_info:", ref_info
    
    # looping over 'figure' objects
    for figure_info in doctree.traverse(figure):
        
        print "\nDEBUG - figure_info:", figure_info
        
        if app.builder.name != 'latex' and app.config.number_figures:
            for cap in figure_info.traverse(caption):
                cap[0] = Text("%s %d: %s" % (app.config.figure_caption_prefix, i, cap[0]))

        for id in figure_info['ids']:
            figids[id] = i
        i += 1

    # replace numfig nodes with links
    if app.builder.name != 'latex':
        for ref_info in doctree.traverse(num_ref):
            
            #print "\nDEBUG - ref_info:", ref_info
            
            if '#' in ref_info['reftarget']:
                label, target = ref_info['reftarget'].split('#')
                labelfmt = label + " %d"
            else:
                labelfmt = '%d'
                target = ref_info['reftarget']

            if app.builder.name == 'html':
                

                # custom hack                
                if "/" in ref_info['refdoc']:
                    print "\nATTENTION! Small hack to the original one, otherwise links to figure do not work because of DBE folder structure (An extra folder name is put in front of the link by default) <rbianchi>."
                    link = ref_info['refdoc'].split("/")[1] + '.html#' + target
                # original here below
                else:
                    link = ref_info['refdoc']+'.html#'+target
                    
                #print "DEBUG - target: %r - ref: %r" % (target, link)
                

                # check if targets are found, otherwise prompt an error message                
                if target in figids:
                    html = '<a href="%s">%s</a>' % (link, labelfmt %(figids[target]))
                    ref_info.replace_self(raw(html, html, format='html'))
                else:
                    print "\nERROR!! label %r not found! Check it, please." % target
                    print "Here below the list of the labels which have been found in the document."
                    print "[pay attention: underscores '_' should be substituted with hyphens '-' and capital letters with small ones]"
                    print "figids:", pprint.pformat(figids)
                    print
            else:
                ref_info.replace_self(Text(labelfmt % (figids[target])))
